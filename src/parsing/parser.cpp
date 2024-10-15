#include "parser.h"

#include <magic_enum/magic_enum.hpp>

namespace jcc::parsing {
    ActionRowElement Parser::GetAction(tokenizer::Token const &token) {
        auto const id{[&] {
            auto const tokenId{token.GetValueType()};

            if (std::holds_alternative<tokenizer::GenericType>(tokenId) &&
                std::get<tokenizer::GenericType>(tokenId) ==
                        tokenizer::GenericType::Identifier) {
                return 0;
            }

            if (std::holds_alternative<tokenizer::Punctuator>(tokenId)) {
                switch (std::get<tokenizer::Punctuator>(tokenId)) {
                    case tokenizer::Punctuator::Plus:
                        return 1;
                    case tokenizer::Punctuator::Asterisk:
                        return 2;
                    case tokenizer::Punctuator::LeftParenthesis:
                        return 3;
                    case tokenizer::Punctuator::RightParenthesis:
                        return 4;
                    default:
                        break;
                }
            }

            if (std::holds_alternative<tokenizer::SpecialPurpose>(tokenId) &&
                std::get<tokenizer::SpecialPurpose>(tokenId) ==
                        tokenizer::SpecialPurpose::EndOfFile) {
                return 5;
            }

            throw std::runtime_error{"Invalid token"};
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

        switch (auto const &[index, action]{GetAction(token)}; action) {
            case ActionRowElement::Action::Shift:
                m_NonTerminals.emplace(token);
                m_States.push(index);
                std::cout << "shift: " << index << '\n';
                ++m_CurrentIt;
                break;
            case ActionRowElement::Action::Reduce: {
                for (int i = 0; i < c_Grammar[index - 1].second; ++i) {
                    m_NonTerminals.pop();
                    m_States.pop();
                }
                // non-terminal
                auto const nonTerminal{c_Grammar[index - 1].first};
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
