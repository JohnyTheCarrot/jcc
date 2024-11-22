#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "production.hpp"

using Grammar = std::vector<jcc::parser_gen::Production>;

using TerminalMap = std::unordered_map<
        jcc::parser_gen::Symbol, jcc::parser_gen::TerminalSet>;

TerminalMap GenerateFirstTable(
        std::vector<jcc::parser_gen::NonTerminal> const &nonTerminals,
        std::vector<jcc::parser_gen::Terminal> const    &terminals,
        Grammar const                                   &grammar
) {
    TerminalMap table;

    // copy terminals to table
    std::ranges::transform(
            terminals, std::inserter(table, table.end()),
            [](jcc::parser_gen::Terminal const &nonTerminal) {
                return std::make_pair(
                        &nonTerminal, jcc::parser_gen::TerminalSet{&nonTerminal}
                );
            }
    );

    // Write empty set to table for each terminal
    for (std::size_t i{}; i < nonTerminals.size(); ++i) {
        table[&nonTerminals.at(i)] = {};
    }

    // add epsilon to table
    table[&jcc::parser_gen::NonTerminal::c_SPrime] = {};

    bool didChange{true};

    while (didChange) {
        didChange = false;

        for (auto const &[nonTerminal, symbols, index] : grammar) {
            auto      &firstSet{table.at(nonTerminal)};
            auto const nFirstSetElements{firstSet.size()};

            int const numSymbols{static_cast<int>(symbols.size())};
            for (int i{}; i < numSymbols; ++i) {
                auto const &currentSymbol{symbols[i]};

                auto const &firstEntry{table.at(currentSymbol)};
                std::ranges::copy_if(
                        firstEntry, std::inserter(firstSet, firstSet.end()),
                        [](jcc::parser_gen::Terminal const *nonTerminal) {
                            return !nonTerminal->m_Token.empty();
                        }
                );

                auto const containsEpsilon{firstEntry.contains(
                        &jcc::parser_gen::Terminal::c_Epsilon
                )};

                if (!containsEpsilon) {
                    break;
                }

                if (i == numSymbols - 1) {
                    firstSet.emplace(&jcc::parser_gen::Terminal::c_Epsilon);
                }
            }

            didChange |= firstSet.size() != nFirstSetElements;
        }
    }

    return table;
}

TerminalMap
GenerateFollowTable(TerminalMap const &firstTable, Grammar const &grammar) {
    TerminalMap table;

    // Write empty set to table for each terminal
    for (auto const &terminal : firstTable | std::views::keys) {
        table[terminal] = {};
    }

    table[&jcc::parser_gen::NonTerminal::c_SPrime] = {
            &jcc::parser_gen::Terminal::c_Eof
    };

    bool didChange{true};
    while (didChange) {
        didChange = false;

        for (auto const &production : grammar) {
            auto const &[nonTerminal, symbols, index]{production};
            for (auto it{symbols.cbegin()}; it != symbols.cend(); ++it) {
                auto const &symbol{*it};
                if (!std::holds_alternative<
                            jcc::parser_gen::NonTerminal const *>(symbol)) {
                    continue;
                }

                auto      &currentFollowSet{table.at(symbol)};
                auto const nFirstSetElements{currentFollowSet.size()};
                auto const betaContainsEpsilon{[&] {
                    assert(it + 1 != symbols.end());
                    auto const beta{*std::next(it)};
                    return firstTable.at(beta).contains(
                            &jcc::parser_gen::Terminal::c_Epsilon
                    );
                }};

                if (it + 1 == symbols.end() || betaContainsEpsilon()) {
                    std::ranges::copy_if(
                            table.at(nonTerminal),
                            std::inserter(
                                    currentFollowSet, currentFollowSet.end()
                            ),
                            [](jcc::parser_gen::Terminal const *nonTerminal) {
                                return !nonTerminal->IsEpsilon();
                            }
                    );
                } else {
                    auto const  beta{*std::next(it)};
                    auto const &betaFirstSet{firstTable.at(beta)};


                    std::ranges::copy_if(
                            betaFirstSet,
                            std::inserter(
                                    currentFollowSet, currentFollowSet.end()
                            ),
                            [](jcc::parser_gen::Terminal const *nonTerminal) {
                                return !nonTerminal->IsEpsilon();
                            }
                    );

                    if (std::ranges::find_if(
                                betaFirstSet,
                                [](auto const &terminal) {
                                    return terminal->IsEpsilon();
                                }
                        ) != betaFirstSet.end()) {
                        std::ranges::copy_if(
                                table.at(nonTerminal),
                                std::inserter(
                                        currentFollowSet, currentFollowSet.end()
                                ),
                                [](jcc::parser_gen::Terminal const *nonTerminal
                                ) { return !nonTerminal->IsEpsilon(); }
                        );
                    }
                }

                didChange |= currentFollowSet.size() != nFirstSetElements;
            }
        }
    }

    return table;
}

jcc::parser_gen::TerminalSet ComputeLookahead(
        jcc::parser_gen::Item const &item, TerminalMap const &firstTable
) {
    jcc::parser_gen::TerminalSet lookahead;

    auto const  beta{item.GetSymbolsAfterPosition()};
    auto const &currentLookahead{item.m_LookAhead};

    bool                         canDeriveEpsilon{true};
    jcc::parser_gen::TerminalSet betaFirst{};
    for (auto const &symbol : beta) {
        if (!canDeriveEpsilon) {
            break;
        }
        auto const &firstSet{firstTable.at(symbol)};

        betaFirst.insert(firstSet.cbegin(), firstSet.cend());
        canDeriveEpsilon =
                firstSet.contains(&jcc::parser_gen::Terminal::c_Epsilon);
    }
    lookahead.insert(betaFirst.cbegin(), betaFirst.cend());

    // If beta can derive epsilon, add the current lookahead
    if (canDeriveEpsilon) {
        lookahead.insert(currentLookahead.cbegin(), currentLookahead.cend());
    }

    return lookahead;
}

jcc::parser_gen::ItemSet
Closure(Grammar const &grammar, jcc::parser_gen::ItemSet const &itemSet,
        TerminalMap const &firstTable) {
    jcc::parser_gen::ItemSet closure{itemSet};

    bool didChange = true;
    while (didChange) {
        didChange = false;

        for (auto currentClosure{closure}; auto const &item : currentClosure) {
            // Get the symbol after the dot (if any)
            auto const nextSymbol{item.GetSymbolAtPosition()};
            if (!std::holds_alternative<jcc::parser_gen::NonTerminal const *>(
                        nextSymbol
                )) {
                continue;// Skip if no symbol after dot or not a non-terminal
            }

            auto const *nonTerminal =
                    std::get<jcc::parser_gen::NonTerminal const *>(nextSymbol);

            // Add items for all productions of the non-terminal
            for (auto const &production : grammar) {
                if (production.m_Terminal != nonTerminal) {
                    continue;
                }

                jcc::parser_gen::TerminalSet lookahead{
                        ComputeLookahead(item, firstTable)
                };

                if (jcc::parser_gen::Item newItem{
                            &production, 0, std::move(lookahead)
                    };
                    closure.insert(newItem).second) {
                    didChange = true;
                }
            }
        }
    }

    return closure;
}

jcc::parser_gen::ItemSet
Goto(jcc::parser_gen::ItemSet const &itemSet,
     jcc::parser_gen::Symbol const &symbol, Grammar const &grammar,
     TerminalMap const &firstTable) {
    jcc::parser_gen::ItemSet gotoSet;

    for (auto const &item : itemSet) {
        if (auto const nextSymbol{item.GetSymbolAtPosition()};
            nextSymbol != symbol) {
            continue;
        }

        jcc::parser_gen::Item newItem{
                item.m_Production, item.m_Position + 1, item.m_LookAhead
        };

        gotoSet.insert(std::move(newItem));
    }

    return Closure(grammar, gotoSet, firstTable);
}

using GotoMap = std::vector<std::unordered_map<jcc::parser_gen::Symbol, int>>;

std::vector<jcc::parser_gen::ItemSet> GenerateCanonicalSetCollection(
        std::vector<jcc::parser_gen::NonTerminal> const &nonTerminals,
        std::vector<jcc::parser_gen::Terminal> const    &terminals,
        Grammar const &grammar, TerminalMap const &firstTable, GotoMap &gotoMap
) {
    std::vector items{
            Closure(grammar,
                    {jcc::parser_gen::Item{
                            &grammar[0], 0, {&jcc::parser_gen::Terminal::c_Eof}
                    }},
                    firstTable)
    };

    std::vector<jcc::parser_gen::Symbol> grammarSymbols;
    for (auto const &nonTerminal : nonTerminals) {
        grammarSymbols.emplace_back(&nonTerminal);
    }

    for (auto const &terminal : terminals) {
        grammarSymbols.emplace_back(&terminal);
    }

    while (true) {
        auto const nItems{items.size()};
        for (std::size_t i{}; i < items.size(); ++i) {
            auto const        &itemSet{items[i]};
            GotoMap::reference gotoMapEntry{
                    i >= gotoMap.size() ? gotoMap.emplace_back() : gotoMap[i]
            };
            std::vector<jcc::parser_gen::ItemSet> resultSet;
            for (auto const &symbol : grammarSymbols) {
                auto gotoSet{Goto(itemSet, symbol, grammar, firstTable)};
                if (!gotoSet.empty() &&
                    std::ranges::find(items, gotoSet) == items.cend()) {

                    if (!std::holds_alternative<std::nullptr_t>(symbol)) {
                        gotoMapEntry[symbol] = static_cast<int>(
                                items.size() + resultSet.size()
                        );
                    }
                    resultSet.emplace_back(std::move(gotoSet));
                }
            }

            items.insert(items.end(), resultSet.begin(), resultSet.end());
        }
        if (items.size() == nItems) {
            break;
        }
    }

    return items;
}

enum class ActionType { Shift, Reduce, Accept, Error };

struct Action final {
    ActionType m_Type;
    int        m_Value;

    Action(ActionType shift, int value)
        : m_Type{shift}
        , m_Value{value} {
    }

    [[nodiscard]]
    bool
    operator==(Action const &other) const {
        return m_Type == other.m_Type && m_Value == other.m_Value;
    }
};

struct ParsingTableRow final {
    using Actions =
            std::unordered_map<jcc::parser_gen::Terminal const *, Action>;

    Actions m_Actions;
    std::unordered_map<jcc::parser_gen::NonTerminal const *, std::optional<int>>
            m_Goto;
};

using ParsingTable = std::vector<ParsingTableRow>;

ParsingTable GenerateParsingTable(
        std::vector<jcc::parser_gen::ItemSet> const     &canonicalSetCollection,
        GotoMap const                                   &gotoMap,
        std::vector<jcc::parser_gen::NonTerminal> const &nonTerminals
) {
    ParsingTable table;

    auto const verifyInsertion{[](auto &currentRow, auto const &pair) {
        auto const &[_, result]{currentRow.m_Actions.emplace(pair)};
        if (!result && currentRow.m_Actions.at(pair.first) != pair.second) {
            throw std::runtime_error{"Invalid grammar, conflicts found"};
        }
    }};

    for (int i{}; i < static_cast<int>(canonicalSetCollection.size()); ++i) {
        auto const &itemSet{canonicalSetCollection[i]};
        auto       &currentRow{table.emplace_back()};

        // GOTO
        for (auto const &nonTerminal : nonTerminals) {
            if (gotoMap.at(i).contains(&nonTerminal)) {
                currentRow.m_Goto[&nonTerminal] =
                        gotoMap.at(i).at(&nonTerminal);
            }
        }

        // ACTION
        for (auto const &item : itemSet) {
            if (!item.HasNextSymbol()) {
                if (item.m_Production->m_Terminal ==
                    &jcc::parser_gen::NonTerminal::c_SPrime) {
                    verifyInsertion(
                            currentRow,
                            std::make_pair(
                                    &jcc::parser_gen::Terminal::c_Eof,
                                    Action{ActionType::Accept, 0}
                            )
                    );
                    continue;
                }

                for (auto const &terminal : item.m_LookAhead) {
                    verifyInsertion(
                            currentRow,
                            std::make_pair(
                                    terminal, Action{ActionType::Reduce,
                                                     item.m_Production->m_Index}
                            )
                    );
                }
            }
            auto const &symbol{item.GetSymbolAtPosition()};
            if (!std::holds_alternative<jcc::parser_gen::Terminal const *>(
                        symbol
                )) {
                continue;
            }

            auto const it{gotoMap.at(i).find(
                    std::get<jcc::parser_gen::Terminal const *>(symbol)
            )};
            if (it == gotoMap.at(i).end()) {
                continue;
            }
            auto const &[_sym, shiftIndex]{*it};
            verifyInsertion(
                    currentRow,
                    std::make_pair(
                            std::get<jcc::parser_gen::Terminal const *>(symbol),
                            Action{ActionType::Shift, shiftIndex}
                    )
            );
        }
    }

    return table;
}

void OutputActionCPlusPlus(
        jcc::parser_gen::Terminal const &terminal,
        ParsingTableRow::Actions const &actions, std::ostream &os
) {
    if (!actions.contains(&terminal)) {
        os << "Error()";
    } else {
        auto const &action{actions.at(&terminal)};
        switch (action.m_Type) {
            case ActionType::Shift:
                os << "Shift(" << action.m_Value << ")";
                break;
            case ActionType::Reduce:
                os << "Reduce(" << action.m_Value << ")";
                break;
            case ActionType::Accept:
                os << "Accept()";
                break;
            case ActionType::Error:
                os << "Error()";
                break;
        }
    }
}

void OutputCPlusPlusLRTable(
        ParsingTable const                              &table,
        std::vector<jcc::parser_gen::Terminal> const    &terminals,
        std::vector<jcc::parser_gen::NonTerminal> const &nonTerminals,
        std::string const &lrTableCppPath, std::string const &lrTableHeaderPath
) {
    std::ofstream file{lrTableHeaderPath, std::ios::trunc};

    auto const numActionElements{terminals.size() + 1};

    file << "#ifndef LR_TABLE_H\n"
         << "#define LR_TABLE_H\n\n"
         << "#include \"parsing/lrtable_utils.hpp\"\n"
         << "#include <array>\n#include \"tokenizer/token.h\"\n\n"
         << "namespace jcc::parser {\n"
         << "\tusing ActionHeader = std::array<jcc::tokenizer::Token::Type, "
         << numActionElements << ">;\n"
         << "\textern ActionHeader const c_ActionHeader;\n\n"
         << "\tusing Table = LrOneTable<" << numActionElements << ", "
         << nonTerminals.size() << ", " << table.size() << ">;\n"
         << "\textern Table const c_LrOneTable;"
         << "\n"
         << "}// namespace jcc::parsing\n"
         << "#endif//LR_TABLE_H\n";

    std::ofstream cppFile{lrTableCppPath, std::ios::trunc};

    cppFile << "#include \"lr_table.hpp\"\n\n"
            << "namespace jcc::parser {\n"
            << "\tActionHeader const c_ActionHeader{";
    for (auto const &terminal : terminals) {
        cppFile << terminal.m_Token;
        if (&terminal != &terminals.back()) {
            cppFile << ", ";
        }
    }

    cppFile << "};\n\n"
            << "\tTable const c_LrOneTable{[] {\n"
            << "\t\tTable table{};\n\n";

    for (std::size_t i{}; i < table.size(); ++i) {
        auto const &row{table[i]};
        cppFile << "\t\ttable[" << i << "] = std::make_pair<ActionRow<"
                << numActionElements << ">, GotoRow<" << nonTerminals.size()
                << ">>(\n"
                << "\t\t\tActionRow<" << numActionElements << ">{";

        for (auto const &terminal : terminals) {
            OutputActionCPlusPlus(terminal, row.m_Actions, cppFile);
            cppFile << ", ";
        }
        OutputActionCPlusPlus(
                jcc::parser_gen::Terminal::c_Eof, row.m_Actions, cppFile
        );
        cppFile << "}, GotoRow<" << nonTerminals.size() << ">{";

        for (auto const &nonTerminal : nonTerminals) {
            if (auto it{row.m_Goto.find(&nonTerminal)};
                it != row.m_Goto.end() && it->second) {
                cppFile << it->second.value();
            } else {
                cppFile << "std::nullopt";
            }

            if (&nonTerminal != &nonTerminals.back()) {
                cppFile << ", ";
            }
        }

        cppFile << "}\n"
                << "\t\t);\n";
    }

    cppFile << "\n"
            << "\t\treturn table;\n"
            << "\t}()};\n"
            << "}// namespace jcc::parser\n";
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <cpp_output_file> <hpp_output_file>\n";
        exit(1);
    }

    using namespace jcc::parser_gen;

    // Define Terminals
    std::vector terminals{
            Terminal{"jcc::tokenizer::Punctuator::Plus"},            // +
            Terminal{"jcc::tokenizer::Punctuator::LeftParenthesis"}, // (
            Terminal{"jcc::tokenizer::Punctuator::RightParenthesis"},// )
            Terminal{"jcc::tokenizer::GenericType::PpNumber"}        // n
    };

    // Define NonTerminals
    std::vector nonTerminals{
            NonTerminal{"E"},// Expression
            NonTerminal{"T"} // Term
    };

    // Define Grammar Productions
    Grammar const grammar{
            // S → E
            Production{&NonTerminal::c_SPrime, {&nonTerminals[0]}, 0},

            // E → T
            Production{&nonTerminals[0], {&nonTerminals[1]}, 1},

            // E → ( E )
            Production{
                    &nonTerminals[0],
                    {&terminals[1], &nonTerminals[0], &terminals[2]},
                    2
            },

            // T → n
            Production{&nonTerminals[1], {&terminals[3]}, 3},

            // T → + T
            Production{&nonTerminals[1], {&terminals[0], &nonTerminals[1]}, 4},

            // T → T + n
            Production{
                    &nonTerminals[1],
                    {&nonTerminals[1], &terminals[0], &terminals[3]},
                    5
            },
    };
    std::cout << "Grammar:" << std::endl;
    std::ranges::copy(
            grammar,
            std::ostream_iterator<jcc::parser_gen::Production>{std::cout, "\n"}
    );
    auto const firstTable{GenerateFirstTable(nonTerminals, terminals, grammar)};
    std::cout << "\nFirst Table:" << std::endl;
    for (auto const &[symbol, firstSet] : firstTable) {
        if (std::holds_alternative<jcc::parser_gen::Terminal const *>(symbol)) {
            continue;
        }
        std::cout << "FIRST(" << GetSymbolName(symbol) << ") = {";
        for (auto const &terminal : firstSet) {
            std::cout << terminal->m_Token << ", ";
        }
        std::cout << "}\n";
    }

    auto const followTable{GenerateFollowTable(firstTable, grammar)};
    std::cout << "\nFollow Table:" << std::endl;
    for (auto const &[symbol, followSet] : followTable) {
        if (std::holds_alternative<jcc::parser_gen::Terminal const *>(symbol)) {
            continue;
        }
        std::cout << "FOLLOW(" << GetSymbolName(symbol) << ") = {";
        for (auto const &terminal : followSet) {
            std::cout << terminal->m_Token << ", ";
        }
        std::cout << "}\n";
    }

    auto const closure{
            Closure(grammar,
                    {jcc::parser_gen::Item{
                            &grammar[0], 0, {&jcc::parser_gen::Terminal::c_Eof}
                    }},
                    firstTable)
    };
    std::cout << "\nClosure:\n" << closure << std::endl;

    GotoMap    gotoMap;
    auto const items{GenerateCanonicalSetCollection(
            nonTerminals, terminals, grammar, firstTable, gotoMap
    )};

    // print item set
    auto i{0};
    for (auto const &itemSet : items) {
        std::cout << "I" << i << ":\n" << itemSet << std::endl;
        ++i;
    }

    auto const parsingTable{GenerateParsingTable(items, gotoMap, nonTerminals)};
    std::cout << "\nParsing Table:" << std::endl;
    for (int i{}; i < static_cast<int>(parsingTable.size()); ++i) {
        auto const &row{parsingTable[i]};
        std::cout << "State " << i << ":\n";
        std::cout << "- Actions:\n";
        for (auto const &[terminal, action] : row.m_Actions) {
            std::cout << "  " << terminal->m_Token << " -> ";
            switch (action.m_Type) {
                case ActionType::Shift:
                    std::cout << "Shift " << action.m_Value;
                    break;
                case ActionType::Reduce:
                    std::cout << "Reduce " << action.m_Value;
                    break;
                case ActionType::Accept:
                    std::cout << "Accept";
                    break;
                case ActionType::Error:
                    std::cout << "Error";
                    break;
            }
            std::cout << std::endl;
        }

        std::cout << "- Goto:\n";
        for (auto const &[nonTerminal, value] : row.m_Goto) {
            std::cout << "  " << nonTerminal->m_Name << " -> ";
            if (value.has_value()) {
                std::cout << value.value();
            } else {
                std::cout << "Error";
            }
            std::cout << std::endl;
        }
    }

    std::string const cppOutputFile{argv[1]};
    std::string const hppOutputFile{argv[2]};
    OutputCPlusPlusLRTable(
            parsingTable, terminals, nonTerminals, cppOutputFile, hppOutputFile);
}
