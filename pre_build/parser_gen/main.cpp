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

using TerminalSet = std::unordered_set<jcc::parser_gen::Terminal const *>;

using TerminalMap = std::unordered_map<jcc::parser_gen::Symbol, TerminalSet>;

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
                return std::make_pair(&nonTerminal, TerminalSet{&nonTerminal});
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

jcc::parser_gen::ItemSet
Closure(Grammar const &grammar, jcc::parser_gen::ItemSet const &itemSet,
        TerminalMap const &firstTable) {
    jcc::parser_gen::ItemSet closure{itemSet};

    bool didChange = true;
    while (didChange) {
        didChange = false;

        for (auto currentClosure = closure; auto const &item : currentClosure) {
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

                if (jcc::parser_gen::Item newItem{&production, 0};
                    closure.insert(newItem).second) {
                    didChange = true;
                }
            }
        }
    }

    return closure;
}

int main() {
    std::vector terminals{jcc::parser_gen::Terminal{"+"},
                          jcc::parser_gen::Terminal{"*"},
                          jcc::parser_gen::Terminal{"("},
                          jcc::parser_gen::Terminal{")"},
                          jcc::parser_gen::Terminal{"id"},
                          jcc::parser_gen::Terminal::c_Epsilon};

    std::vector nonTerminals{
            jcc::parser_gen::NonTerminal{"E"},
            jcc::parser_gen::NonTerminal{"E'"},
            jcc::parser_gen::NonTerminal{"T"},
            jcc::parser_gen::NonTerminal{"T'"},
            jcc::parser_gen::NonTerminal{"F"},
    };

    Grammar const grammar{
            jcc::parser_gen::Production{
                    &jcc::parser_gen::NonTerminal::c_SPrime, {&nonTerminals[0]}
            },
            jcc::parser_gen::Production{
                    &nonTerminals[0], {&nonTerminals[2], &nonTerminals[1]}
            },
            jcc::parser_gen::Production{
                    &nonTerminals[1],
                    {&terminals[0], &nonTerminals[2], &nonTerminals[1]}
            },
            jcc::parser_gen::Production{&nonTerminals[1], {&terminals[5]}},
            jcc::parser_gen::Production{
                    &nonTerminals[2], {&nonTerminals[4], &nonTerminals[3]}
            },
            jcc::parser_gen::Production{
                    &nonTerminals[3],
                    {&terminals[1], &nonTerminals[4], &nonTerminals[3]}
            },
            jcc::parser_gen::Production{&nonTerminals[3], {&terminals[5]}},
            jcc::parser_gen::Production{
                    &nonTerminals[4],
                    {&terminals[2], &nonTerminals[0], &terminals[3]}
            },
            jcc::parser_gen::Production{&nonTerminals[4], {&terminals[4]}}
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

    auto const closure{Closure(
            grammar, {jcc::parser_gen::Item{&grammar[0], 0}}, firstTable
    )};
    std::cout << "\nClosure:\n" << closure << std::endl;
}
