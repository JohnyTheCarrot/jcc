#include "parser.h"

#include <format>
#include <magic_enum/magic_enum.hpp>

#include "../../pre_build/parser_gen/production.hpp"
#include "lr_table.hpp"

namespace jcc::parsing {
    void PrintTo(NonTerminalAstNode const &node, int depth, std::ostream &os) {
        os << std::string(depth, ' ') << magic_enum::enum_name(node.m_Type)
           << " {\n";

        for (auto const &child : node.m_Children) {
            PrintTo(child, depth + 2, os);
            os << ",\n";
        }

        os << std::format("{:>{}}}}", "", depth);
    }

    void PrintTo(AstNode const &node, int depth, std::ostream &os) {
        if (std::holds_alternative<tokenizer::Token>(node)) {
            os << std::string(depth, ' ');
            PrintTo(std::get<tokenizer::Token>(node).GetValueType(), &os);
        } else {
            PrintTo(std::get<NonTerminalAstNode>(node), depth, os);
        }
    }

    ActionRowElement Parser::GetAction(tokenizer::Token const &token) {
        auto const id{[&] {
            auto const tokenType{token.GetValueType()};

            // TODO: in the final grammar, we won't really need this check
            if (!c_TokenToIndexMap.contains(tokenType)) {
                throw std::runtime_error{"Token not in grammar"};
            }

            return c_TokenToIndexMap.at(tokenType);
        }()};

        return c_LrOneTable.at(m_States.top()).first.at(id);
    }

    Parser::Parser(preprocessor::Preprocessor &preprocessor)
        : m_CurrentIt{preprocessor.begin()}
        , m_Preprocessor{&preprocessor} {
        m_States.push(0);
    }

    bool Parser::ReadOutput(AstNode &ast) {
        auto const &token{[this] {
            if (m_CurrentIt == m_Preprocessor->end()) {
                return tokenizer::Token{
                        tokenizer::SpecialPurpose::EndOfFile,
                        m_Preprocessor->GetCurrentSpan()
                };
            };

            return *m_CurrentIt;
        }()};

        std::cout << "token: " << token << '\n';

        switch (auto const &[index, action]{GetAction(token)}; action) {
            case ActionRowElement::Action::Shift:
                m_NonTerminals.emplace(token);
                m_States.push(index);
                std::cout << "shift: " << index << '\n';
                ++m_CurrentIt;
                break;
            case ActionRowElement::Action::Reduce: {
                auto const nonTerminal{c_GrammarLengths.at(index).first};
                std::cout << "reduce: " << magic_enum::enum_name(nonTerminal)
                          << '\n';

                NonTerminalAstNode node{nonTerminal};
                for (int i = 0; i < c_GrammarLengths.at(index).second; ++i) {
                    node.m_Children.push_back(std::move(m_NonTerminals.top()));
                    m_NonTerminals.pop();
                    m_States.pop();
                }
                // TODO: work away this reverse
                std::reverse(node.m_Children.begin(), node.m_Children.end());

                m_NonTerminals.emplace(std::move(node));
                ast = m_NonTerminals.top();

                m_States.emplace(c_LrOneTable.at(m_States.top())
                                         .second
                                         .at(static_cast<int>(nonTerminal))
                                         .value());
                std::cout << "goto: " << m_States.top() << '\n';
                break;
            }
            case ActionRowElement::Action::Error: {
                // Collect expected tokens
                std::unordered_set<std::string> expectedTokensSet;
                auto const &expectedTokens{c_LrOneTable[m_States.top()].first};
                for (std::size_t i{}; i < expectedTokens.size(); ++i) {
                    auto const &row{expectedTokens[i]};
                    if (row.action != ActionRowElement::Action::Error) {
                        std::stringstream ss;
                        PrintTo(c_Terminals.at(i), &ss);
                        expectedTokensSet.emplace(ss.str());
                    }
                }

                std::stringstream expectedSs;
                for (auto it{expectedTokensSet.begin()};
                     it != expectedTokensSet.end(); ++it) {
                    expectedSs << *it;

                    if (std::next(it) != expectedTokensSet.end()) {
                        expectedSs << ", ";
                    }
                }

                std::stringstream tokenSs;
                tokenSs << token;

                throw std::runtime_error{std::format(
                        "Unexpected token: {}\nExpected one of: {}",
                        tokenSs.str(), expectedSs.str()
                )};
            }
            case ActionRowElement::Action::Accept:
                std::cout << "accept\n";
                return false;
        }

        return true;
    }
}// namespace jcc::parsing
