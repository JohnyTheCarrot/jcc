#include "preprocessor.h"

#include <format>// for format
#include <iostream>
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
            bool executeCommands, bool expandMacros, bool isGlueRhs
    ) {
        while (true) {
            if (auto ppToken{GetNextFromTokenizer(
                        executeCommands, expandMacros, isGlueRhs
                )};
                !pp_token::GetToken(ppToken).Is(
                        tokenizer::SpecialPurpose::NewLine
                )) {
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
            if (auto token{pp_token::GetToken(SimpleTokenRead())};
                token.Is(tokenizer::SpecialPurpose::NewLine)) {
                return;
            }
        }
    }

    PreprocessorToken Preprocessor::GetNextFromTokenizer(
            bool executeCommands, bool expandMacros, bool isGlueRhs
    ) {
        if (!m_Buffer.empty()) {
            auto ppToken{std::move(m_Buffer.front())};
            m_Buffer.pop();
            std::cout << "Popping from buffer: "
                      << pp_token::GetToken(ppToken).ToString() << std::endl;
            return ppToken;
        }

        while (true) {
            auto ppToken{SimpleTokenRead(expandMacros)};
            auto [token, rhs] = [&]
                    -> std::pair<
                            tokenizer::Token, std::optional<tokenizer::Token>> {
                if (auto const *glue{std::get_if<GluePreprocessorToken>(&ppToken
                    )}) {
                    return {glue->GetLhs(), glue->GetRhs()};
                }

                return {std::get<BasicPreprocessorToken>(ppToken).GetToken(),
                        std::nullopt};
            }();
            token.m_IsGlueRhs = isGlueRhs;

            if (auto [value, span, isGlueRhs]{token};
                std::holds_alternative<tokenizer::SpecialPurpose>(value)) {
                switch (std::get<tokenizer::SpecialPurpose>(value)) {
                    case tokenizer::SpecialPurpose::EndOfFile:
                        return ppToken;
                    case tokenizer::SpecialPurpose::InvalidEscape: {
                        auto &compilerState{parsing::CompilerState::GetInstance(
                        )};
                        auto const &tokenSpan{pp_token::GetToken(ppToken).m_Span
                        };
                        compilerState.EmplaceDiagnostic<
                                diagnostics::EscapeWithoutNewline>(
                                tokenSpan.m_Source, tokenSpan.m_Span
                        );
                        continue;
                    }
                    default:
                        break;
                }
            }

            if (!executeCommands)
                return ppToken;

            auto const &commandMap{
                    commands::PreprocessorCommandSingleton::GetInstance()
                            .GetCommandMap()
            };

            auto executeToken = [&commandMap,
                                 this](auto &&token
                                ) -> std::optional<PreprocessorToken> {
                auto const valueType{token.GetValueType()};

                if (auto const commandIt{commandMap.find(valueType)};
                    commandIt != commandMap.end()) {
                    if (auto result{commandIt->second->Execute(
                                *this, std::move(token)
                        )};
                        result.has_value()) {
                        // If the command does not return a token, it means it was a directive.
                        return std::move(result.value());
                    }
                    return std::nullopt;
                }
                return BasicPreprocessorToken{std::move(token)};
            };

            static int id{0};
            int        currentId{id};
            static int depth{0};
            ++id;
            for (auto c{0}; c < depth - 1; ++c) std::cout << "|\t";
            std::cout << '(' << currentId
                      << ") LHS before execution: " << token.ToString()
                      << std::endl;
            ++depth;
            auto lhs{executeToken(std::move(token))};
            --depth;
            if (!lhs.has_value()) {
                continue;
            }
            for (auto c{0}; c < depth - 1; ++c) std::cout << "|\t";
            std::cout << '(' << currentId << ") LHS after execution: "
                      << pp_token::GetToken(lhs.value()).ToString()
                      << std::endl;

            if (!rhs.has_value())
                return lhs.value();

            std::vector<PreprocessorToken> tokens;
            auto const &argReader{m_pMacroStore->GetCurrentMacroInvocation()
                                          ->m_CurrentArgReader};
            while (argReader.has_value() &&
                   argReader->m_CurrentTokenIndex + 1 <
                           static_cast<int>(argReader->m_Args.size())) {
                tokens.emplace_back(std::move(lhs.value()));
                auto lhsToken{pp_token::GetToken(SimpleTokenRead())};
                lhsToken.m_IsGlueRhs = true;
                lhs                  = executeToken(lhsToken);
            }

            rhs->m_IsGlueRhs = true;
            for (auto c{0}; c < depth - 1; ++c) std::cout << "|\t";
            std::cout << '(' << currentId << ") RHS: " << rhs->ToString()
                      << std::endl;
            auto rhsExecutedPp{executeToken(rhs.value())};
            if (!rhsExecutedPp.has_value())
                throw std::runtime_error{"illegal glue"};

            auto const &resultToken{pp_token::GetToken(lhs.value())};
            std::string merged{std::format(
                    "{}{}", resultToken.ToString(),
                    pp_token::GetToken(rhsExecutedPp.value()).ToString()
            )};
            assert(merged != "(_END");
            std::cout << "Glued tokens: " << merged << std::endl;
            std::istringstream   mergedStream{merged};
            tokenizer::Tokenizer mergerTokenizer{mergedStream};
            auto                 mergedToken{
                    executeToken(mergerTokenizer.GetNextToken().value()).value()
            };

            for (auto &tk : tokens) { m_Buffer.emplace(std::move(tk)); }

            m_Buffer.emplace(std::move(mergedToken));

            auto result = m_Buffer.front();
            m_Buffer.pop();
            return result;
        }
    }

    PreprocessorToken Preprocessor::SimpleTokenRead(bool expandMacros) {
        while (true) {
            if (expandMacros) {
                if (auto token{m_pMacroStore->GetTokenFromMacroArgumentReader()
                    };
                    token.has_value()) {
                    // used to be true for COMMA_MACRO_NOT_A_DELIMITER, but broke stuff
                    return BasicPreprocessorToken{std::move(token.value())};
                }

                if (auto token{m_pMacroStore->GetTokenFromMacroStack()};
                    token.has_value()) {
                    // used to be true for COMMA_MACRO_NOT_A_DELIMITER, but broke stuff
                    return std::move(token.value());
                }
            }

            auto const &tokenizer{m_TokenizerStack.top().GetTokenizer()};
            auto       &tokenIter{m_TokenizerStack.top().GetTokenIter()};

            if (tokenIter == tokenizer.end()) {
                if (m_TokenizerStack.size() == 1) {
                    return BasicPreprocessorToken{
                            {tokenizer::SpecialPurpose::EndOfFile,
                             tokenizer.GetLastSpan()}
                    };
                }

                m_TokenizerStack.pop();
                continue;
            }

            auto token{*tokenIter};
            ++tokenIter;

            if (tokenIter != tokenizer.end() &&
                tokenIter->Is(tokenizer::Punctuator::HashHash)) {
                ++tokenIter;
                if (tokenIter == tokenizer.end()) {
                    throw std::runtime_error{"Unexpected end of file after ##"};
                }
                auto rhs{pp_token::GetToken(SimpleTokenRead())};
                rhs.m_IsGlueRhs = true;

                return GluePreprocessorToken{
                        BasicPreprocessorToken{std::move(token)}, std::move(rhs)
                };
            }

            return BasicPreprocessorToken{std::move(token)};
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
        auto token{pp_token::GetToken(SimpleTokenRead())};

        while (token.Is(tokenizer::SpecialPurpose::NewLine)) {
            token = pp_token::GetToken(SimpleTokenRead());
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

    void Preprocessor::ReinsertToken(PreprocessorToken &&token) {
        m_Buffer.emplace(std::move(token));
    }
}// namespace jcc::preprocessor
