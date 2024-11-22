#include "parser.h"

#include <magic_enum/magic_enum.hpp>

#include "lr_table.hpp"

namespace jcc::parsing {
    ActionRowElement Parser::GetAction(tokenizer::Token const &token) {
        auto const id{[&] {
            auto const tokenType{token.GetValueType()};

            // TODO: in the final grammar, we won't really need this check
            if (!c_TokenToIndexMap.contains(tokenType)) {
                throw std::runtime_error{"Token not in grammar"};
            }

            return c_TokenToIndexMap.at(tokenType);
        }()};

        return c_LrOneTable[m_States.top()].first[id];
    }

    Parser::Parser(preprocessor::Preprocessor &preprocessor)
        : m_CurrentIt{preprocessor.begin()}
        , m_Preprocessor{&preprocessor} {
        m_States.push(0);
    }

    codegen::BinaryOutput Parser::ReadOutput() {
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
                for (int i = 0; i < c_GrammarLengths[index].second; ++i) {
                    m_NonTerminals.pop();
                    m_States.pop();
                }
                // non-terminal
                auto const nonTerminal{c_GrammarLengths[index].first};
                std::cout << "reduce: " << magic_enum::enum_name(nonTerminal)
                          << '\n';

                m_NonTerminals.emplace(nonTerminal);
                // goto
                m_States.emplace(c_LrOneTable[m_States.top()]
                                         .second[static_cast<int>(nonTerminal)]
                                         .value());
                break;
            }
            case ActionRowElement::Action::Error:
                throw std::runtime_error{"Syntax error"};
            case ActionRowElement::Action::Accept:
                std::cout << "accept\n";
                return '1';
        }

        return '0';
    }
}// namespace jcc::parsing
