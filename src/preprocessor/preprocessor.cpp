#include "preprocessor.h"

#include <format>       // for format
#include <optional>     // for opt...
#include <unordered_map>// for ope...
#include <utility>      // for move
#include <variant>      // for hash

#include "diagnostics/variants/tk_pp/escape_without_newline.hpp"// for Esc...
#include "diagnostics/variants/tk_pp/orphaned_endif.hpp"        // for Orp...
#include "parsing/parser.h"                                     // for Com...
#include "preprocessor/commands/command.h"                      // for Pre...
#include "preprocessor/macro_store.h"                           // for Mac...
#include "preprocessor/preprocessor_iterator.h"                 // for Pre...
#include "preprocessor/preprocessor_token.h"                    // for Pre...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::preprocessor {
    PreprocessorToken Preprocessor::GetNextPreprocessorToken(
            bool executeCommands, bool expandMacros
    ) {
        while (true) {
            if (auto ppToken{GetNextFromTokenizer(executeCommands, expandMacros)
                };
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
            bool executeCommands, bool expandMacros
    ) {
        while (true) {
            auto ppToken{SimpleTokenRead(expandMacros)};

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
    }

    PreprocessorToken Preprocessor::SimpleTokenRead(bool expandMacros) {
        while (true) {
            if (expandMacros) {
                if (auto token{m_pMacroStore->GetTokenFromMacroArgumentReader()
                    };
                    token.has_value()) {

                    // used to be true for COMMA_MACRO_NOT_A_DELIMITER, but broke stuff
                    return {std::move(token.value()), false};
                }

                if (auto token{m_pMacroStore->GetTokenFromMacroStack()};
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
