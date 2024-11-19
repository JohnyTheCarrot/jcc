#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "production.hpp"

using Grammar = std::vector<jcc::parser_gen::Production>;

using TerminalSet = std::unordered_set<jcc::parser_gen::Terminal const *>;

using FirstTable = std::unordered_map<jcc::parser_gen::Symbol, TerminalSet>;

FirstTable GenerateFirstTable(
        std::vector<jcc::parser_gen::NonTerminal> const &nonTerminals,
        std::vector<jcc::parser_gen::Terminal> const    &terminals,
        Grammar const                                   &grammar
) {
    FirstTable table;

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
    jcc::parser_gen::NonTerminal const *const epsilon{nullptr};
    table[epsilon] = {};

    bool didChange{true};

    while (didChange) {
        didChange = false;

        for (auto const &[terminal, symbols] : grammar) {
            auto      &firstSet{table.at(terminal)};
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

jcc::parser_gen::Symbol AfterDot(jcc::parser_gen::Item const &item) {
    auto const &prod{*item.m_Production};

    if (static_cast<std::size_t>(item.m_Position) >= prod.m_Symbols.size()) {
        return nullptr;
    }

    return prod.m_Symbols[item.m_Position];
}

// TODO: understand and refactor
jcc::parser_gen::ItemSet
Predict(Grammar const                                           &grammar,
        std::vector<jcc::parser_gen::Item> /* copy intentional*/ items) {
    jcc::parser_gen::ItemSet prediction;
    prediction.reserve(items.size());
    std::ranges::copy(items, std::inserter(prediction, prediction.end()));

    std::size_t previousPredictionSize{prediction.size()};
    while (!items.empty()) {
        auto const symbol{AfterDot(items.back())};
        items.pop_back();

        for (auto const &production : grammar) {
            if (production.m_Terminal == nullptr ||
                !std::holds_alternative<jcc::parser_gen::NonTerminal const *>(
                        symbol
                ) ||
                std::get<jcc::parser_gen::NonTerminal const *>(symbol) !=
                        production.m_Terminal) {
                continue;
            }

            prediction.emplace(&production, 0);
            if (previousPredictionSize >= prediction.size()) {
                continue;
            }

            previousPredictionSize = prediction.size();
            items.emplace_back(&production, 0);
        }
    }

    return prediction;
}

using Partitions =
        std::unordered_map<jcc::parser_gen::Symbol, jcc::parser_gen::ItemSet>;

Partitions Partition(jcc::parser_gen::ItemSet const &itemSet) {
    Partitions partitions;

    for (auto const &item : itemSet) {
        auto const symbol{AfterDot(item)};

        if (std::holds_alternative<std::nullptr_t>(symbol)) {
            partitions[nullptr].emplace(item);
            continue;
        }

        partitions[symbol].emplace(item.m_Production, item.m_Position + 1);
    }

    return partitions;
}

auto ComputeItemSets(Grammar const &grammar) {
    std::vector<jcc::parser_gen::ItemSet> fullItemSets{};
    std::vector<jcc::parser_gen::ItemSet> itemSets{
            {jcc::parser_gen::Item{&grammar[0], 0}}
    };
    std::unordered_map<jcc::parser_gen::ItemSet const *, std::size_t>
            itemSetIndices;
    for (std::size_t i{}; i < itemSets.size(); ++i) {
        itemSetIndices[&itemSets[i]] = i;
    }

    std::vector<std::unordered_map<jcc::parser_gen::Symbol, std::size_t>>
                                          shifts;
    std::vector<jcc::parser_gen::ItemSet> reductions;

    for (std::size_t i{}; i < itemSets.size(); ++i) {
        auto const &itemSet{itemSets[i]};
        auto const  predictionSet{
                Predict(grammar, std::vector(itemSet.cbegin(), itemSet.cend()))
        };
        fullItemSets.push_back(predictionSet);

        std::unordered_map<jcc::parser_gen::Symbol, std::size_t> setShifts;
        jcc::parser_gen::ItemSet                                 setReductions;

        auto const partitions{Partition(predictionSet)};
        for (auto const &[symbol, items] : partitions) {
            if (std::holds_alternative<std::nullptr_t>(symbol)) {
                setReductions.insert(items.cbegin(), items.cend());
                continue;
            }

            std::size_t const j{[&] {
                if (itemSetIndices.contains(&items)) {
                    return itemSetIndices[&items];
                }

                itemSets.push_back(items);
                return itemSetIndices[&items] = itemSets.size();
            }()};
            setShifts[symbol] = j;
        }
        shifts.emplace_back(std::move(setShifts));
        reductions.emplace_back(std::move(setReductions));
    }

    // print results
    for (std::size_t i{}; i < fullItemSets.size(); ++i) {
        std::cout << i << ":\n" << fullItemSets[i] << '\n';
    }
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
            jcc::parser_gen::Production{nullptr, {&nonTerminals[0]}},
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

    // std::ranges::copy(
    //         grammar,
    //         std::ostream_iterator<jcc::parser_gen::Production>{std::cout, "\n"}
    // );

    // ComputeItemSets(grammar);
    // jcc::parser_gen::ItemSet const itemSet{Predict(grammar, {{&grammar[0], 0}})
    // };
    //
    // std::cout << "0:\n" << itemSet << '\n';
    //
    // auto const partitions{Partition(itemSet)};
    // for (auto const &[symbol, items] : partitions) {
    //     std::cout << GetSymbolName(symbol) << ":\n";
    //     std::ranges::copy(
    //             items,
    //             std::ostream_iterator<jcc::parser_gen::Item>{std::cout, "\n"}
    //     );
    //     std::cout << '\n';
    // }
}
