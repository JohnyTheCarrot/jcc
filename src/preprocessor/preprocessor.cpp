#include "preprocessor.h"

#include <algorithm>    // for copy
#include <optional>     // for optional
#include <unordered_map>// for operator==, _Node_it...
#include <utility>      // for move, pair
#include <variant>      // for get, holds_alternative

#include "preprocessor/commands/command.h"     // for PreprocessorCommandS...
#include "preprocessor/macro_store.h"          // for MacroStore
#include "preprocessor/preprocessor_iterator.h"// for PreprocessorIterator
#include "preprocessor/preprocessor_token.h"   // for PreprocessorToken

namespace jcc::preprocessor {
    PreprocessorToken Preprocessor::GetNextPreprocessorToken() {
        while (true) {
            if (auto ppToken{GetNextFromTokenizer()};
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

    tokenizer::Token Preprocessor::SkipUntilConditionEnd() {
        auto &tokenizer{m_TokenizerStack.top().GetTokenizer()};
        return tokenizer.SkipUntilConditionEnd();
    }

    void Preprocessor::PushConditional() {
        ++m_ConditionalDepth;
    }

    void Preprocessor::PopConditional() {
        if (m_ConditionalDepth == 0) {
            // TODO: diagnosis
            throw FatalCompilerError{
                    // Diagnosis::Kind::PP_OrphanedConditionalClosure,
                    // GetCurrentSpan()
            };
        }

        --m_ConditionalDepth;
    }

    PreprocessorToken Preprocessor::GetNextFromTokenizer(bool executeCommands) {
        while (true) {
            auto ppToken{SimpleTokenRead()};

            if (auto &[token, span]{ppToken.m_Token};
                std::holds_alternative<tokenizer::SpecialPurpose>(token)) {
                switch (std::get<tokenizer::SpecialPurpose>(token)) {
                    case tokenizer::SpecialPurpose::EndOfFile:
                        return ppToken;
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

            if (auto const command{commandMap.find(valueType)};
                command != commandMap.end()) {
                if (auto result{command->second->Execute(
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

    PreprocessorToken Preprocessor::SimpleTokenRead() {
        while (true) {
            if (auto token{m_pMacroStore->GetTokenFromMacroArgumentReader()};
                token.has_value()) {

                // used to be true for COMMA_MACRO_NOT_A_DELIMITER, but broke stuff
                return {std::move(token.value()), false};
            }

            if (auto token{m_pMacroStore->GetTokenFromMacroStack()};
                token.has_value()) {

                // used to be true for COMMA_MACRO_NOT_A_DELIMITER, but broke stuff
                return {std::move(token.value()), false};
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

    void Preprocessor::EmitWarning(Diagnosis &&diagnosis) const {
        m_pDiagnoses->emplace_back(std::move(diagnosis));
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

    Preprocessor::Preprocessor(
            std::string const &filename, Diagnosis::Vec &diagnoses
    )
        : m_TokenizerStack{[&] {
            tokenizer::Tokenizer  tokenizer{filename};
            TokenizerIteratorPair pair{std::move(tokenizer)};

            std::stack<TokenizerIteratorPair> stack;
            stack.push(std::move(pair));

            return stack;
        }()}
        , m_pDiagnoses{&diagnoses} {
    }

    void Preprocessor::OpenHeader(std::string_view filename) {
        try {
            auto &top{m_TokenizerStack.emplace(
                    tokenizer::Tokenizer{std::string{filename}}
            )};
            ++top.GetTokenIter();
        } catch (tokenizer::TokenizerFileOpenFailure const &) {
            // TODO: diagnosis
            throw FatalCompilerError{
                    // Diagnosis::Kind::PP_InclDirectiveFileOpenFailed,
                    // GetCurrentSpan()
            };
        }
    }
}// namespace jcc::preprocessor
