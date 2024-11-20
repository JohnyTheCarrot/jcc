#include <algorithm>
#include <cassert>
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

        for (auto const &[nonTerminal, symbols] : grammar) {
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
            auto const &[nonTerminal, symbols]{production};
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

    // Apply Closure on the fully constructed gotoSet
    gotoSet.merge(Closure(grammar, gotoSet, firstTable));

    return gotoSet;
}

int main() {
    // std::vector terminals{jcc::parser_gen::Terminal{"+"},
    //                       jcc::parser_gen::Terminal{"*"},
    //                       jcc::parser_gen::Terminal{"("},
    //                       jcc::parser_gen::Terminal{")"},
    //                       jcc::parser_gen::Terminal{"id"},
    //                       jcc::parser_gen::Terminal::c_Epsilon};
    //
    // std::vector nonTerminals{
    //         jcc::parser_gen::NonTerminal{"E"},
    //         jcc::parser_gen::NonTerminal{"E'"},
    //         jcc::parser_gen::NonTerminal{"T"},
    //         jcc::parser_gen::NonTerminal{"T'"},
    //         jcc::parser_gen::NonTerminal{"F"},
    // };
    std::vector terminals{
            jcc::parser_gen::Terminal{"c"}, jcc::parser_gen::Terminal{"d"}
    };
    std::vector nonTerminals{
            jcc::parser_gen::NonTerminal{"S"}, jcc::parser_gen::NonTerminal{"C"}
    };

    // Grammar const grammar{
    //         jcc::parser_gen::Production{
    //                 &jcc::parser_gen::NonTerminal::c_SPrime, {&nonTerminals[0]}
    //         },
    //         jcc::parser_gen::Production{
    //                 &nonTerminals[0], {&nonTerminals[2], &nonTerminals[1]}
    //         },
    //         jcc::parser_gen::Production{
    //                 &nonTerminals[1],
    //                 {&terminals[0], &nonTerminals[2], &nonTerminals[1]}
    //         },
    //         jcc::parser_gen::Production{&nonTerminals[1], {&terminals[5]}},
    //         jcc::parser_gen::Production{
    //                 &nonTerminals[2], {&nonTerminals[4], &nonTerminals[3]}
    //         },
    //         jcc::parser_gen::Production{
    //                 &nonTerminals[3],
    //                 {&terminals[1], &nonTerminals[4], &nonTerminals[3]}
    //         },
    //         jcc::parser_gen::Production{&nonTerminals[3], {&terminals[5]}},
    //         jcc::parser_gen::Production{
    //                 &nonTerminals[4],
    //                 {&terminals[2], &nonTerminals[0], &terminals[3]}
    //         },
    //         jcc::parser_gen::Production{&nonTerminals[4], {&terminals[4]}}
    // };
    Grammar const grammar{
            // S' -> S
            jcc::parser_gen::Production{
                    &jcc::parser_gen::NonTerminal::c_SPrime, {&nonTerminals[0]}
            },
            // S -> CC
            jcc::parser_gen::Production{
                    &nonTerminals[0], {&nonTerminals[1], &nonTerminals[1]}
            },
            // C -> cC
            jcc::parser_gen::Production{
                    &nonTerminals[1], {&terminals[0], &nonTerminals[1]}
            },
            // C -> d
            jcc::parser_gen::Production{&nonTerminals[1], {&terminals[1]}}
    };
    // Grammar const grammar{
    //         // E' -> E
    //         jcc::parser_gen::Production{&nonTerminals[1], {&nonTerminals[0]}},
    //         // E -> E + T
    //         jcc::parser_gen::Production{
    //                 &nonTerminals[0],
    //                 {&nonTerminals[0], &terminals[0], &nonTerminals[2]}
    //         },
    //         // E -> T
    //         jcc::parser_gen::Production{&nonTerminals[0], {&nonTerminals[2]}},
    //         // T -> T * F
    //         jcc::parser_gen::Production{
    //                 &nonTerminals[2],
    //                 {&nonTerminals[2], &terminals[1], &nonTerminals[4]}
    //         },
    //         // T -> F
    //         jcc::parser_gen::Production{&nonTerminals[2], {&nonTerminals[4]}},
    //         // F -> ( E )
    //         jcc::parser_gen::Production{
    //                 &nonTerminals[4],
    //                 {&terminals[2], &nonTerminals[0], &terminals[3]}
    //         },
    //         // F -> id
    //         jcc::parser_gen::Production{&nonTerminals[4], {&terminals[4]}}
    // };

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
        std::cout << "Items:\n";
        auto const nItems{items.size()};
        for (auto itemsCopy{items}; auto const &itemSet : itemsCopy) {
            std::cout << "ItemSet:\n" << itemSet << std::endl;
            for (auto const &symbol : grammarSymbols) {
                auto gotoSet{Goto(itemSet, symbol, grammar, firstTable)};
                if (!gotoSet.empty() &&
                    std::ranges::find(items, gotoSet) == items.cend()) {
                    items.emplace_back(std::move(gotoSet));
                }
            }
        }
        if (items.size() == nItems) {
            break;
        }
    }

    // print item set
    auto i{0};
    for (auto const &itemSet : items) {
        std::cout << "I" << i << ":\n" << itemSet << std::endl;
        ++i;
    }
}
