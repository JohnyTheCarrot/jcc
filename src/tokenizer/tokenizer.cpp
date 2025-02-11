#include "tokenizer/tokenizer.h"

#include <algorithm>// for find_if_not
#include <cctype>   // for isdigit, isspace
#include <fstream>  // for basic_ifstream, ifstream
#include <string>   // for basic_string, char_traits
#include <utility>  // for move

#include "misc/Diagnosis.h"              // for Diagnosis, FatalCompilerError
#include "tokenizer/char_iter.h"         // for CharIter, CharInfo
#include "tokenizer/tokenizer_iterator.h"// for TokenizerIterator
#include "tokens/character_constants.h"  // for Tokenize
#include "tokens/identifiers.h"          // for CollectRestOfIdentifier
#include "tokens/pp_numbers.h"           // for Tokenize
#include "tokens/static_tokens.h"        // for StaticTokenTokenizationResult
#include "tokens/string_literals.h"      // for Tokenize

namespace jcc::tokenizer {
    bool Tokenizer::SkipWhitespace() {
        auto newIt{std::find_if_not(
                m_CharIter, CharIter::c_UntilNewline,
                [](CharInfo const &charInfo) {
                    return std::isspace(charInfo.m_Char);
                }
        )};

        bool const didSkipWhitespace{m_CharIter != newIt};
        m_CharIter = newIt;

        return didSkipWhitespace;
    }

    void Tokenizer::SkipLineComment() {
        while (m_CharIter != CharIter::c_UntilNewline) ++m_CharIter;
    }

    void Tokenizer::SkipBlockComment(Span &span) {
        while (true) {
            if (m_CharIter == CharIter::end()) {
                span.m_Span.set_end(m_CharIter.GetSentinel().m_LastPos);
                // TODO: diagnosis
                throw FatalCompilerError{
                        // Diagnosis::Kind::UnexpectedEOF, std::move(span)
                };
            }
            if (m_CharIter->m_Char == '*') {
                ++m_CharIter;
                if (m_CharIter == CharIter::end()) {
                    span.m_Span.set_end(m_CharIter.GetSentinel().m_LastPos);

                    // TODO: diagnosis
                    throw FatalCompilerError{
                            // Diagnosis::Kind::UnexpectedEOF, std::move(span)
                    };
                }

                if (m_CharIter->m_Char == '/') {
                    ++m_CharIter;
                    return;
                }
            }

            ++m_CharIter;
        }
    }

    std::optional<Token> Tokenizer::TokenizeStaticToken(
            static_tokens::StaticTokenTokenizationResult::ValueOrString const
                    &valueOrString,
            Span &span, bool skippedWhitespace
    ) {
        auto const tokenValue{[&]() -> Token::Value {
            if (!std::holds_alternative<Token::Value>(valueOrString))
                // TODO: diagnosis
                throw FatalCompilerError{
                        // Diagnosis::Kind::TK_UnexpectedChar, std::move(span),
                        // m_CharIter->m_Char
                };

            auto value{std::get<Token::Value>(valueOrString)};
            if (std::holds_alternative<Punctuator>(value)) {
                if (auto const punctuator{std::get<Punctuator>(value)};
                    punctuator == Punctuator::PpLeftParenthesis &&
                    skippedWhitespace)
                    value = Punctuator::LeftParenthesis;
            }

            return value;
        }()};

        if (std::holds_alternative<Punctuator>(tokenValue)) {
            if (auto const punctuator{std::get<Punctuator>(tokenValue)};
                punctuator == Punctuator::Dot &&
                m_CharIter != CharIter::end() &&
                std::isdigit(m_CharIter->m_Char)) {
                return pp_numbers::Tokenize(
                        m_CharIter, span.m_Span.start(), true
                );
            }
        }

        if (std::holds_alternative<SpecialPurpose>(tokenValue)) {
            auto const specialPurpose{std::get<SpecialPurpose>(tokenValue)};

            if (specialPurpose == SpecialPurpose::LineComment) {
                SkipLineComment();
                return GetNextToken();
            }

            if (specialPurpose == SpecialPurpose::BlockComment) {
                SkipBlockComment(span);
                return GetNextToken();
            }
        }

        return Token{.m_Value = tokenValue, .m_Span = std::move(span)};
    }

    Tokenizer::Tokenizer(std::string const &fileName)
        : m_Source{std::make_shared<diagnostics::Source>(
                  [&] {
                      auto const ifstream{
                              std::make_unique<std::ifstream>(fileName)
                      };

                      if (!ifstream->is_open())
                          throw TokenizerFileOpenFailure{};

                      return std::string{
                              std::istreambuf_iterator{*ifstream},
                              std::istreambuf_iterator<char>{}
                      };
                  }(),
                  fileName
          )}
        , m_Input{m_Source->m_Buffer}
        , m_CharIter{m_Input, m_Source} {
    }

    Tokenizer::Tokenizer(Tokenizer &&other) noexcept
        : m_Source{std::move(other.m_Source)}
        , m_Input{std::move(other.m_Input)}
        , m_CharIter{std::move(other.m_CharIter)} {
        m_CharIter.SetInput(m_Input);
    }

    Tokenizer &Tokenizer::operator=(Tokenizer &&other) noexcept {
        if (this == &other)
            return *this;

        m_Input    = std::move(other.m_Input);
        m_CharIter = std::move(other.m_CharIter);
        m_CharIter.SetInput(m_Input);

        return *this;
    }

    // TODO: this function is too long, refactor it
    std::optional<Token> Tokenizer::GetNextToken() {
        if (m_CharIter == CharIter::end())
            return std::nullopt;

        bool const skippedWhitespace{SkipWhitespace()};

        if (m_CharIter == CharIter::end())
            return std::nullopt;

        Span span{
                m_CharIter.GetSource(),
                {m_CharIter.GetCurrentPos(), m_CharIter.GetCurrentPos()}
        };

        switch (m_CharIter->m_Char) {
            case '\n':
                ++m_CharIter;
                return Token{SpecialPurpose::NewLine, span};
            case '\'':
                ++m_CharIter;
                return character_constants::Tokenize(
                        m_CharIter, ConstantPrefix::None, span.m_Span.start()
                );
            case '"':
                ++m_CharIter;
                return string_literals::Tokenize(
                        m_CharIter, ConstantPrefix::None, span.m_Span.start()
                );
            default:
                break;
        }

        if (m_CharIter != CharIter::end() && std::isdigit(m_CharIter->m_Char)) {
            return pp_numbers::Tokenize(m_CharIter, span.m_Span.start(), false);
        }

        bool const couldBeIdentifier{
                Identifier::IsValidFirstChar(m_CharIter->m_Char)
        };
        auto [valueOrString,
              trieResultEndMarker]{static_tokens::Tokenize(m_CharIter)};
        span.m_Span.set_end(trieResultEndMarker);

        if (std::holds_alternative<std::string>(valueOrString)) {
            auto identifier{std::get<std::string>(valueOrString)};

            span.m_Span.set_end(
                    identifiers::CollectRestOfIdentifier(m_CharIter, identifier)
                            .value_or(span.m_Span.end())
            );

            if (m_CharIter != CharIter::end()) {
                auto const nextChar{m_CharIter->m_Char};

                if (nextChar == '\'') {
                    ++m_CharIter;
                    ConstantPrefix const prefix{ToConstantPrefix(identifier)};
                    return character_constants::Tokenize(
                            m_CharIter, prefix, span.m_Span.start()
                    );
                }

                if (nextChar == '"') {
                    ++m_CharIter;
                    ConstantPrefix const prefix{ToConstantPrefix(identifier)};
                    return string_literals::Tokenize(
                            m_CharIter, prefix, span.m_Span.start()
                    );
                }
            }

            if (couldBeIdentifier)
                return identifiers::Tokenize(
                        m_CharIter,
                        identifiers::IdentifierTokenStart{
                                .m_Identifier = std::move(identifier),
                                .m_Start      = span.m_Span.start(),
                        }
                );
        }

        return TokenizeStaticToken(valueOrString, span, skippedWhitespace);
    }

    TokenizerIterator Tokenizer::begin() {
        return TokenizerIterator{*this};
    }

    TokenizerIterator Tokenizer::end() const {
        return TokenizerIterator{};
    }

    Span Tokenizer::GetLastSpan() const {
        return {m_CharIter.GetSource(),
                {m_CharIter.GetCurrentPos() - 1, m_CharIter.GetCurrentPos()}};
    }

    std::optional<Token> Tokenizer::SkipUntilConditionEnd() {
        int conditionDepth{1};

        while (true) {
            SkipWhitespace();

            if (m_CharIter == CharIter::end()) {
                return std::nullopt;
            }

            if (m_CharIter->m_Char == '\n') {
                ++m_CharIter;
                continue;
            }

            Span span{
                    // m_CharIter.GetFileName(), m_CharIter.GetCurrentSpanMarker(),
                    // m_CharIter.GetCurrentSpanMarker(), m_CharIter.GetInput()
                    m_CharIter.GetSource(),
                    {m_CharIter.GetCurrentPos(), m_CharIter.GetCurrentPos()}
            };

            if (m_CharIter->m_Char == '#') {
                auto const token{
                        static_tokens::TokenizeKeywordsAndDirectives(m_CharIter)
                };

                if (!std::holds_alternative<Token::Value>(token.valueOrString
                    )) {
                    continue;
                }

                auto const directive{std::get<Token::Value>(token.valueOrString)
                };
                if (!std::holds_alternative<Directive>(directive)) {
                    continue;
                }

                span.m_Span.set_end(token.endPos);

                switch (std::get<Directive>(directive)) {
                    case Directive::If:
                    case Directive::Ifdef:
                    case Directive::Ifndef:
                        ++conditionDepth;
                        break;
                    case Directive::Endif:
                        --conditionDepth;
                        if (conditionDepth != 0)
                            continue;

                        [[fallthrough]];
                    case Directive::Elif:
                    case Directive::Else:
                    case Directive::Elifdef:
                    case Directive::Elifndef:
                        return Token{directive, std::move(span)};
                    default:
                        break;
                }

                // read until newline
                while (m_CharIter != CharIter::c_UntilNewline) ++m_CharIter;

                if (m_CharIter == CharIter::end()) {
                    return std::nullopt;
                }

                ++m_CharIter;

                if (m_CharIter == CharIter::end()) {
                    return std::nullopt;
                }
            }
        }

        assert(false);
    }
}// namespace jcc::tokenizer
