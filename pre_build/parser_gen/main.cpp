#include <algorithm>
#include <cassert>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "grammar_parser.hpp"
#include "production.hpp"

using TerminalMap = std::unordered_map<
        jcc::parser_gen::Symbol, jcc::parser_gen::TerminalSet,
        jcc::parser_gen::SymbolHash>;

TerminalMap GenerateFirstTable(
        std::vector<jcc::parser_gen::NonTerminal> const &nonTerminals,
        std::vector<jcc::parser_gen::Terminal> const    &terminals,
        jcc::parser_gen::Grammar const                  &grammar
) {
    TerminalMap table;

    // copy terminals to table
    // std::ranges::transform(
    //         terminals, std::inserter(table, table.end()),
    //         [](jcc::parser_gen::Terminal const &nonTerminal) {
    //             return std::make_pair(
    //                     &nonTerminal, jcc::parser_gen::TerminalSet{&nonTerminal}
    //             );
    //         }
    // );
    for (std::size_t i{}; i < terminals.size(); ++i) {
        jcc::parser_gen::TerminalIndex terminalIndex{i};
        table[terminalIndex].insert(terminalIndex);
    }

    // Write empty set to table for each terminal
    for (std::size_t i{}; i < nonTerminals.size(); ++i) {
        jcc::parser_gen::NonTerminalIndex nonTerminalIndex{i};
        table[nonTerminalIndex] = {};
    }

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
                        [&](jcc::parser_gen::TerminalIndex terminalIndex) {
                            auto const &terminal{
                                    terminals.at(terminalIndex.m_Index)
                            };
                            return !terminal.m_Token.empty();
                        }
                );

                auto const containsEpsilon{firstEntry.contains(
                        jcc::parser_gen::TerminalIndex::c_EpsilonIndex
                )};

                if (!containsEpsilon) {
                    break;
                }

                if (i == numSymbols - 1) {
                    firstSet.emplace(
                            jcc::parser_gen::TerminalIndex::c_EpsilonIndex
                    );
                }
            }

            didChange |= firstSet.size() != nFirstSetElements;
        }
    }

    return table;
}

TerminalMap GenerateFollowTable(
        jcc::parser_gen::Terminals const &terminals,
        TerminalMap const &firstTable, jcc::parser_gen::Grammar const &grammar
) {
    TerminalMap table;

    // Write empty set to table for each terminal
    for (auto const &terminal : firstTable | std::views::keys) {
        table[terminal] = {};
    }

    table[jcc::parser_gen::NonTerminalIndex::c_SPrimeIndex] = {
            jcc::parser_gen::TerminalIndex::c_EofIndex
    };

    bool didChange{true};
    while (didChange) {
        didChange = false;

        for (auto const &production : grammar) {
            auto const &[nonTerminal, symbols, index]{production};
            for (auto it{symbols.cbegin()}; it != symbols.cend(); ++it) {
                auto const &symbol{*it};
                if (!std::holds_alternative<jcc::parser_gen::NonTerminalIndex>(
                            symbol
                    )) {
                    continue;
                }

                auto      &currentFollowSet{table.at(symbol)};
                auto const nFirstSetElements{currentFollowSet.size()};
                auto const betaContainsEpsilon{[&] {
                    assert(it + 1 != symbols.end());
                    auto const beta{*std::next(it)};
                    return firstTable.at(beta).contains(
                            jcc::parser_gen::TerminalIndex::c_EpsilonIndex
                    );
                }};

                if (it + 1 == symbols.end() || betaContainsEpsilon()) {
                    std::ranges::copy_if(
                            table.at(nonTerminal),
                            std::inserter(
                                    currentFollowSet, currentFollowSet.end()
                            ),
                            [&](jcc::parser_gen::TerminalIndex terminalIndex) {
                                auto const &terminal{
                                        terminals.at(terminalIndex.m_Index)
                                };
                                return !terminal.IsEpsilon();
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
                            [&](jcc::parser_gen::TerminalIndex terminalIndex) {
                                auto const &terminal{
                                        terminals.at(terminalIndex.m_Index)
                                };
                                return !terminal.IsEpsilon();
                            }
                    );

                    if (std::ranges::find_if(
                                betaFirstSet,
                                [&](jcc::parser_gen::TerminalIndex terminalIndex
                                ) {
                                    auto const &terminal{
                                            terminals.at(terminalIndex.m_Index)
                                    };

                                    return terminal.IsEpsilon();
                                }
                        ) != betaFirstSet.end()) {
                        std::ranges::copy_if(
                                table.at(nonTerminal),
                                std::inserter(
                                        currentFollowSet, currentFollowSet.end()
                                ),
                                [&](jcc::parser_gen::TerminalIndex terminalIndex
                                ) {
                                    auto const &terminal{
                                            terminals.at(terminalIndex.m_Index)
                                    };
                                    return !terminal.IsEpsilon();
                                }
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
        canDeriveEpsilon = firstSet.contains(
                jcc::parser_gen::TerminalIndex::c_EpsilonIndex
        );
    }
    lookahead.insert(betaFirst.cbegin(), betaFirst.cend());

    // If beta can derive epsilon, add the current lookahead
    if (canDeriveEpsilon) {
        lookahead.insert(currentLookahead.cbegin(), currentLookahead.cend());
    }

    return lookahead;
}

jcc::parser_gen::ItemSet
Closure(jcc::parser_gen::Grammar const &grammar,
        jcc::parser_gen::ItemSet const &itemSet,
        TerminalMap const              &firstTable) {
    jcc::parser_gen::ItemSet closure{itemSet};

    // Use a queue-like mechanism to process newly added items
    std::vector<jcc::parser_gen::Item const *> worklist;
    for (auto const &item : closure) { worklist.push_back(&item); }

    while (!worklist.empty()) {
        auto const *currentItem{worklist.back()};
        worklist.pop_back();

        // Get the symbol after the dot (if any)
        auto const nextSymbol{currentItem->GetSymbolAtPosition()};
        if (!std::holds_alternative<jcc::parser_gen::NonTerminalIndex>(
                    nextSymbol
            )) {
            continue;// Skip if no symbol after dot or not a non-terminal
        }

        auto const nonTerminal{
                std::get<jcc::parser_gen::NonTerminalIndex>(nextSymbol)
        };

        // Add items for all productions of the non-terminal
        for (auto const &production : grammar) {
            if (production.m_NonTerminal != nonTerminal) {
                continue;
            }

            jcc::parser_gen::TerminalSet lookahead{
                    ComputeLookahead(*currentItem, firstTable)
            };

            jcc::parser_gen::Item newItem{&production, 0, std::move(lookahead)};
            if (closure.insert(newItem).second) {
                // Add newly added item to the worklist
                worklist.push_back(&(*closure.find(newItem)));
            }
        }
    }

    return closure;
}

jcc::parser_gen::ItemSet
Goto(jcc::parser_gen::ItemSet const &itemSet,
     jcc::parser_gen::Symbol const  &symbol,
     jcc::parser_gen::Grammar const &grammar, TerminalMap const &firstTable) {
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

using GotoMap = std::vector<std::unordered_map<
        jcc::parser_gen::Symbol, int, jcc::parser_gen::SymbolHash>>;

std::vector<jcc::parser_gen::ItemSet> GenerateCanonicalSetCollection(
        std::vector<jcc::parser_gen::NonTerminal> const &nonTerminals,
        std::vector<jcc::parser_gen::Terminal> const    &terminals,
        jcc::parser_gen::Grammar const &grammar, TerminalMap const &firstTable,
        GotoMap &gotoMap
) {
    std::vector items{Closure(
            grammar,
            {jcc::parser_gen::Item{
                    &grammar[0], 0, {jcc::parser_gen::TerminalIndex::c_EofIndex}
            }},
            firstTable
    )};

    std::vector<jcc::parser_gen::Symbol> grammarSymbols;
    for (std::size_t i{}; i < nonTerminals.size(); ++i) {
        grammarSymbols.emplace_back(jcc::parser_gen::NonTerminalIndex{i});
    }

    for (std::size_t i{}; i < terminals.size(); ++i) {
        grammarSymbols.emplace_back(jcc::parser_gen::TerminalIndex{i});
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

                auto const gotoSetEntry{std::ranges::find(items, gotoSet)};
                auto const resultSetEntry{std::ranges::find(resultSet, gotoSet)
                };
                if (!gotoSet.empty()) {
                    if (!std::holds_alternative<std::nullptr_t>(symbol) &&
                        (gotoSetEntry != items.cend() ||
                         resultSetEntry != resultSet.cend())) {
                        auto const gotoSetIndex{static_cast<int>(
                                gotoSetEntry == items.cend()
                                        ? std::distance(
                                                  resultSet.begin(),
                                                  resultSetEntry
                                          )
                                        : std::distance(
                                                  items.begin(), gotoSetEntry
                                          )
                        )};

                        gotoMapEntry[symbol] = static_cast<int>(gotoSetIndex);
                    }
                    if (gotoSetEntry == items.cend() &&
                        resultSetEntry == resultSet.cend()) {
                        resultSet.emplace_back(std::move(gotoSet));
                    }
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
    bool operator==(Action const &other) const {
        return m_Type == other.m_Type && m_Value == other.m_Value;
    }
};

struct ParsingTableRow final {
    using Actions = std::unordered_map<
            jcc::parser_gen::TerminalIndex, Action,
            jcc::parser_gen::TerminalIndexHash>;

    Actions m_Actions;
    std::unordered_map<
            jcc::parser_gen::NonTerminalIndex, std::optional<int>,
            jcc::parser_gen::NonTerminalIndexHash>
            m_Goto;
};

using ParsingTable = std::vector<ParsingTableRow>;

void VerifyInsertion(
        ParsingTableRow                                         &currentRow,
        std::pair<jcc::parser_gen::TerminalIndex, Action> const &newAction
) {
    if (auto it = currentRow.m_Actions.find(newAction.first);
        it != currentRow.m_Actions.end()) {
        // Conflict detected: existing action vs new action
        if (it->second.m_Type == ActionType::Reduce &&
            newAction.second.m_Type == ActionType::Shift) {
            // Prioritize Shift over Reduce
            it->second = newAction.second;
            std::cout << "Shift/Reduce conflict detected and resolved by "
                         "overwriting "
                         "existing Reduce with new Shift"
                      << std::endl;
            return;
        }
        if (it->second.m_Type == ActionType::Shift &&
            newAction.second.m_Type == ActionType::Reduce) {
            // Keep existing Shift, ignore new Reduce
            std::cout << "Shift/Reduce conflict detected and resolved by "
                         "keeping existing Shift"
                      << std::endl;
            return;
        }

        if (it->second == newAction.second) {
            // No conflict, just ignore the new action
            return;
        }

        // Handle other conflicts (e.g., Reduce-Reduce or Shift-Shift)
        throw std::runtime_error{
                "Shift/Reduce conflict detected and not resolved"
        };
    }

    // No conflict, just insert the new action
    currentRow.m_Actions.insert(newAction);
}

ParsingTable GenerateParsingTable(
        std::vector<jcc::parser_gen::ItemSet> const     &canonicalSetCollection,
        GotoMap const                                   &gotoMap,
        std::vector<jcc::parser_gen::NonTerminal> const &nonTerminals
) {
    ParsingTable table;

    for (int i{}; i < static_cast<int>(canonicalSetCollection.size()); ++i) {
        auto const &itemSet{canonicalSetCollection[i]};
        auto       &currentRow{table.emplace_back()};

        // GOTO
        for (jcc::parser_gen::NonTerminalIndex nonTerminalIndex{};
             nonTerminalIndex.m_Index < nonTerminals.size();
             ++nonTerminalIndex.m_Index) {
            if (gotoMap.at(i).contains(nonTerminalIndex)) {
                currentRow.m_Goto[nonTerminalIndex] =
                        gotoMap.at(i).at(nonTerminalIndex);
            }
        }

        // ACTION
        for (auto const &item : itemSet) {
            if (!item.HasNextSymbol()) {
                if (item.m_Production->m_NonTerminal ==
                    jcc::parser_gen::NonTerminalIndex::c_SPrimeIndex) {
                    VerifyInsertion(
                            currentRow,
                            std::make_pair(
                                    jcc::parser_gen::TerminalIndex::c_EofIndex,
                                    Action{ActionType::Accept, 0}
                            )
                    );
                    continue;
                }

                for (auto const &terminal : item.m_LookAhead) {
                    VerifyInsertion(
                            currentRow,
                            std::make_pair(
                                    terminal, Action{ActionType::Reduce,
                                                     item.m_Production->m_Index}
                            )
                    );
                }
                continue;
            }
            auto const &symbol{item.GetSymbolAtPosition()};
            if (!std::holds_alternative<jcc::parser_gen::TerminalIndex>(symbol
                )) {
                continue;
            }

            auto const it{gotoMap.at(i).find(
                    std::get<jcc::parser_gen::TerminalIndex>(symbol)
            )};
            if (it == gotoMap.at(i).end()) {
                throw std::runtime_error{"Invalid grammar, no goto found"};
            }
            auto const &[_sym, shiftIndex]{*it};
            VerifyInsertion(
                    currentRow,
                    std::make_pair(
                            std::get<jcc::parser_gen::TerminalIndex>(symbol),
                            Action{ActionType::Shift, shiftIndex}
                    )
            );
        }
    }

    return table;
}

void OutputActionCPlusPlus(
        jcc::parser_gen::TerminalIndex  terminalIndex,
        ParsingTableRow::Actions const &actions, std::ostream &os
) {
    if (!actions.contains(terminalIndex)) {
        os << "Error()";
    } else {
        auto const &action{actions.at(terminalIndex)};
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
        jcc::parser_gen::Grammar const &grammar, ParsingTable const &table,
        std::vector<jcc::parser_gen::Terminal> const    &terminals,
        std::vector<jcc::parser_gen::NonTerminal> const &nonTerminals,
        std::string const &lrTableCppPath, std::string const &lrTableHeaderPath
) {
    std::ofstream headerFile{lrTableHeaderPath, std::ios::trunc};

    auto const numTerminals{terminals.size()};
    auto const numNonTerminals{nonTerminals.size()};

    headerFile
            << "#ifndef LR_TABLE_H\n"
            << "#define LR_TABLE_H\n\n"
            << "#include \"parsing/lrtable_utils.hpp\"\n"
            << "#include <array>\n#include \"tokenizer/token.h\"\n\n"
            << "namespace jcc::parsing {\n"
            << "\tusing TokenToIndexMap = "
               "std::unordered_map<jcc::tokenizer::Token::Type, std::size_t>;\n"
            << "\textern TokenToIndexMap const c_TokenToIndexMap;\n\n"
            << "\tusing Terminals = std::array<jcc::tokenizer::Token::Type, "
            << numTerminals << ">;\n"
            << "\textern Terminals const c_Terminals;\n\n"
            << "\tenum class NonTerminal {\n";

    for (std::size_t i{}; i < nonTerminals.size(); ++i) {
        auto const &nonTerminal{nonTerminals[i]};
        headerFile << "\t\t" << nonTerminal.m_Name << " = " << i << ",\n";
    }

    headerFile
            << "\t};\n\n"
            << "\tusing ProductionLengthPair = std::pair<NonTerminal, int>;\n"
            << "\tusing ProductionLengths = std::array<ProductionLengthPair, "
            << grammar.size() << ">;\n"
            << "\textern ProductionLengths const c_GrammarLengths;\n"
            << "\n\tusing Table = LrOneTable<" << numTerminals << ", "
            << numNonTerminals << ", " << table.size() << ">;\n"
            << "\textern Table const c_LrOneTable;"
            << "\n"
            << "}// namespace jcc::parsing\n"
            << "#endif//LR_TABLE_H\n";

    std::ofstream cppFile{lrTableCppPath, std::ios::trunc};

    cppFile << "#include \"lr_table.hpp\"\n\n"
            << "namespace jcc::parsing {\n"
            << "\tTerminals const c_Terminals{";
    for (auto const &terminal : terminals) {
        cppFile << terminal.m_Token;

        if (&terminal != &terminals.back()) {
            cppFile << ", ";
        }
    }
    cppFile << "};\n\n";

    cppFile << "\tTokenToIndexMap const c_TokenToIndexMap{\n";

    for (std::size_t i{}; i < terminals.size(); ++i) {
        cppFile << "\t\tstd::pair{" << terminals[i].m_Token << ", " << i
                << "},\n";
    }
    cppFile << "\t};\n\n"
            << "\tProductionLengths const c_GrammarLengths{\n";

    for (std::size_t i{}; i < grammar.size(); ++i) {
        auto const &production{grammar[i]};
        auto const &nonTerminal{nonTerminals.at(production.m_NonTerminal.m_Index
        )};

        cppFile << "\t\tstd::pair{NonTerminal::" << nonTerminal.m_Name << ", "
                << production.m_Symbols.size() << "}";
        if (i != grammar.size() - 1) {
            cppFile << ',';
        }

        cppFile << '\n';
    }

    cppFile << "\t};\n\n"
            << "\tTable constexpr c_LrOneTable{[] {\n"
            << "\t\tTable table{};\n\n";

    for (std::size_t i{}; i < table.size(); ++i) {
        auto const &row{table[i]};
        cppFile << "\t\ttable[" << i << "] = std::make_pair<ActionRow<"
                << numTerminals << ">, GotoRow<" << numNonTerminals << ">>(\n"
                << "\t\t\tActionRow<" << numTerminals << ">{";

        for (jcc::parser_gen::TerminalIndex terminalIndex{};
             terminalIndex.m_Index < terminals.size();
             ++terminalIndex.m_Index) {
            OutputActionCPlusPlus(terminalIndex, row.m_Actions, cppFile);
            if (terminalIndex.m_Index != terminals.size() - 1) {
                cppFile << ", ";
            }
        }
        cppFile << "}, GotoRow<" << numNonTerminals << ">{";

        for (std::size_t j{}; j < nonTerminals.size(); ++j) {
            jcc::parser_gen::NonTerminalIndex nonTerminalIndex{j};

            if (auto it{row.m_Goto.find(nonTerminalIndex)};
                it != row.m_Goto.end() && it->second) {
                cppFile << it->second.value();
            } else {
                cppFile << "std::nullopt";
            }

            if (j != nonTerminals.size() - 1) {
                cppFile << ", ";
            }
        }

        cppFile << "}\n"
                << "\t\t);\n";
    }

    cppFile << "\n"
            << "\t\treturn table;\n"
            << "\t}()};\n"
            << "}// namespace jcc::parsing\n";
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <grammar> <cpp_output_file> <hpp_output_file>\n";
        exit(1);
    }

    std::string const              grammarPath{argv[1]};
    jcc::parser_gen::GrammarParser parser;
    auto const [terminals, nonTerminals, grammar]{parser.Parse(argv[1])};

    auto const firstTable{GenerateFirstTable(nonTerminals, terminals, grammar)};

    GotoMap    gotoMap;
    auto const items{GenerateCanonicalSetCollection(
            nonTerminals, terminals, grammar, firstTable, gotoMap
    )};

    auto const parsingTable{GenerateParsingTable(items, gotoMap, nonTerminals)};

    std::string const cppOutputFile{argv[2]};
    std::string const hppOutputFile{argv[3]};
    OutputCPlusPlusLRTable(
            grammar, parsingTable, terminals, nonTerminals, cppOutputFile,
            hppOutputFile
    );
}
