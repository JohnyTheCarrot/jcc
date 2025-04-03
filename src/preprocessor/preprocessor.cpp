#include "preprocessor.h"

#include <algorithm>// for copy
#include <iostream>
#include <optional>     // for optional
#include <unordered_map>// for operator==, _Node_it...
#include <utility>      // for move, pair
#include <variant>      // for get, holds_alternative

#include "diagnostics/variants/tk_pp/escape_without_newline.hpp"
#include "diagnostics/variants/tk_pp/include_open_failed.hpp"
#include "diagnostics/variants/tk_pp/orphaned_endif.hpp"
#include "parsing/parser.h"
#include "preprocessor/commands/command.h"     // for PreprocessorCommandS...
#include "preprocessor/macro_store.h"          // for MacroStore
#include "preprocessor/preprocessor_iterator.h"// for PreprocessorIterator
#include "preprocessor/preprocessor_token.h"   // for PreprocessorToken

namespace jcc::preprocessor {
    PreprocessorToken Preprocessor::GetNextPreprocessorToken(
            bool executeCommands, bool expandMacros, bool shouldAdvance
    ) {
        while (true) {
            if (auto ppToken{GetNextFromTokenizer(
                        executeCommands, expandMacros, shouldAdvance
                )};
                !ppToken.m_Token.Is(tokenizer::SpecialPurpose::NewLine)) {
                return ppToken;
            }
        }
    }

    PreprocessorIterator Preprocessor::begin() {
        return ++PreprocessorIterator{*this};
    }

    PreprocessorIterator Preprocessor::end() {
        return PreprocessorIterator::end();
    }

    Span Preprocessor::GetCurrentSpan() const noexcept {
        return m_TokenizerStack.top().GetTokenizer().GetLastSpan();
    }

    std::optional<tokenizer::Token> Preprocessor::SkipUntilConditionEnd() {
        auto &tokenizer{m_TokenizerStack.top().GetTokenizer()};
        return tokenizer.SkipUntilConditionEnd();
    }

    void Preprocessor::PushConditional() {
        ++m_ConditionalDepth;
    }

    void Preprocessor::PopConditional() {
        if (m_ConditionalDepth == 0) {
            auto &compilerState{parsing::CompilerState::GetInstance()};
            // TODO: find a better span
            auto span{GetCurrentSpan()};
            compilerState.EmplaceDiagnostic<diagnostics::OrphanedEndif>(
                    std::move(span.m_Source), span.m_Span
            );
            return;
        }

        --m_ConditionalDepth;
    }

    int Preprocessor::GetConditionalDepth() const noexcept {
        return m_ConditionalDepth;
    }

    void Preprocessor::SkipToNextLine() {
        while (true) {
            if (auto [m_Token, m_IsFromMacro]{SimpleTokenRead()};
                m_Token.Is(tokenizer::SpecialPurpose::NewLine)) {
                return;
            }
        }
    }

    PreprocessorToken Preprocessor::GetNextFromTokenizer(
            bool executeCommands, bool expandMacros, bool shouldAdvance
    ) {
        auto token{[&] {
            while (true) {
                auto ppToken{SimpleTokenRead(expandMacros, shouldAdvance)};

                if (auto &[token, span]{ppToken.m_Token};
                    std::holds_alternative<tokenizer::SpecialPurpose>(token)) {
                    switch (std::get<tokenizer::SpecialPurpose>(token)) {
                        case tokenizer::SpecialPurpose::EndOfFile:
                            return ppToken;
                        case tokenizer::SpecialPurpose::InvalidEscape: {
                            auto &compilerState{
                                    parsing::CompilerState::GetInstance()
                            };
                            compilerState.EmplaceDiagnostic<
                                    diagnostics::EscapeWithoutNewline>(
                                    ppToken.m_Token.m_Span.m_Source,
                                    ppToken.m_Token.m_Span.m_Span
                            );
                            continue;
                        }
                        default:
                            break;
                    }
                }

                if (!executeCommands)
                    return ppToken;

                auto const  valueType{ppToken.m_Token.GetValueType()};
                auto const &commandMap{
                        commands::PreprocessorCommandSingleton::GetInstance()
                                .GetCommandMap()
                };

                if (auto const commandIt{commandMap.find(valueType)};
                    commandIt != commandMap.end()) {
                    if (auto result{commandIt->second->Execute(
                                *this, std::move(ppToken.m_Token)
                        )};
                        result.has_value())
                        // If the command does not return a token, it means it was a directive.
                        return std::move(result.value());
                } else {
                    return ppToken;
                }
            }
        }()};

        if (!executeCommands)
            return token;

        auto const macroToken{m_pMacroStore->GetTokenFromMacroStack(false)};
        auto      &tokenIter{m_TokenizerStack.top().GetTokenIter()};
        auto const isEnd{
                tokenIter == m_TokenizerStack.top().GetTokenizer().end()
        };
        if ((!macroToken.has_value() ||
             !macroToken->Is(tokenizer::Punctuator::HashHash)) &&
            (isEnd || !tokenIter->Is(tokenizer::Punctuator::HashHash))) {
            return token;
        }

        std::shared_ptr<diagnostics::Source> source;
        if (macroToken.has_value()) {
            m_pMacroStore->Advance();
            source = macroToken->m_Span.m_Source;
        } else {
            source = token.m_Token.m_Span.m_Source;
            ++tokenIter;
        }

        auto rhs{GetNextFromTokenizer(executeCommands, expandMacros)};

        auto const           merged{std::format(
                "{}{}", token.m_Token.ToString(), rhs.m_Token.ToString()
        )};
        tokenizer::Tokenizer tokenizer{std::move(source), merged};

        return PreprocessorToken{tokenizer.GetNextToken().value(), false};
    }

    PreprocessorToken
    Preprocessor::SimpleTokenRead(bool expandMacros, bool shouldAdvance) {
        while (true) {
            if (expandMacros) {
                if (auto token{m_pMacroStore->GetTokenFromMacroArgumentReader(
                            shouldAdvance
                    )};
                    token.has_value()) {

                    // used to be true for COMMA_MACRO_NOT_A_DELIMITER, but broke stuff
                    return {std::move(token.value()), false};
                }

                if (auto token{
                            m_pMacroStore->GetTokenFromMacroStack(shouldAdvance)
                    };
                    token.has_value()) {

                    // used to be true for COMMA_MACRO_NOT_A_DELIMITER, but broke stuff
                    return {std::move(token.value()), false};
                }
            }

            auto const &tokenizer{m_TokenizerStack.top().GetTokenizer()};
            auto       &tokenIter{m_TokenizerStack.top().GetTokenIter()};

            if (tokenIter == tokenizer.end()) {
                if (m_TokenizerStack.size() == 1) {
                    return {{tokenizer::SpecialPurpose::EndOfFile,
                             tokenizer.GetLastSpan()},
                            false};
                }

                m_TokenizerStack.pop();
                continue;
            }

            auto token{*tokenIter};
            if (shouldAdvance)
                ++tokenIter;

            return {std::move(token), false};
        }
    }

    MacroStore &Preprocessor::GetMacroStore() const noexcept {
        return *m_pMacroStore;
    }

    TokenizerIteratorPair::TokenizerIteratorPair(
            tokenizer::Tokenizer &&tokenizer
    )
        : m_Tokenizer{std::move(tokenizer)}
        , m_TokenIter{m_Tokenizer.begin()} {
    }

    TokenizerIteratorPair::TokenizerIteratorPair(TokenizerIteratorPair &&other)
        : m_Tokenizer{std::move(other.m_Tokenizer)}
        // The tokenizer iterator is a forward iterator, so it's okay to use .begin()
        , m_TokenIter{m_Tokenizer.begin()} {
        // TODO: why are we incrementing the iterator here?
        ++m_TokenIter;
    }

    tokenizer::TokenizerIterator &
    TokenizerIteratorPair::GetTokenIter() noexcept {
        return m_TokenIter;
    }

    tokenizer::Tokenizer const &
    TokenizerIteratorPair::GetTokenizer() const noexcept {
        return m_Tokenizer;
    }

    tokenizer::Tokenizer &TokenizerIteratorPair::GetTokenizer() noexcept {
        return m_Tokenizer;
    }

    tokenizer::TokenizerIterator &
    TokenizerIteratorPair::GetTokenIter() const noexcept {
        return m_TokenIter;
    }

    void Preprocessor::SkipEmptyLines() {
        auto token{SimpleTokenRead()};

        while (token.m_Token.Is(tokenizer::SpecialPurpose::NewLine)) {
            token = SimpleTokenRead();
        }
    }

    Preprocessor::Preprocessor(std::string const &filename)
        : m_TokenizerStack{[&] {
            tokenizer::Tokenizer  tokenizer{filename};
            TokenizerIteratorPair pair{std::move(tokenizer)};

            std::stack<TokenizerIteratorPair> stack;
            stack.push(std::move(pair));

            return stack;
        }()} {
    }

    void Preprocessor::OpenHeader(std::string_view filename) {
        auto &top{m_TokenizerStack.emplace(
                tokenizer::Tokenizer{std::string{filename}}
        )};
        ++top.GetTokenIter();
    }
}// namespace jcc::preprocessor
