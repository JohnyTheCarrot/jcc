#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <memory>
#include <string>

#include "commands/command.h"
#include "macro_store.h"
#include "misc/Diagnosis.h"
#include "preprocessor_iterator.h"
#include "preprocessor_token.h"
#include "tokenizer/tokenizer.h"
#include "tokenizer/tokenizer_iterator.h"

namespace jcc::preprocessor {
    constexpr std::string_view VaArgs{"__VA_ARGS__"};

    class TokenizerIteratorPair final {
        tokenizer::Tokenizer                 m_Tokenizer;
        mutable tokenizer::TokenizerIterator m_TokenIter;

    public:
        explicit TokenizerIteratorPair(tokenizer::Tokenizer &&tokenizer);

        TokenizerIteratorPair(TokenizerIteratorPair const &) = delete;

        TokenizerIteratorPair(TokenizerIteratorPair &&) noexcept;

        [[nodiscard]]
        tokenizer::TokenizerIterator &GetTokenIter() noexcept;

        [[nodiscard]]
        tokenizer::Tokenizer const &GetTokenizer() const noexcept;

        [[nodiscard]]
        tokenizer::Tokenizer &GetTokenizer() noexcept;

        [[nodiscard]]
        tokenizer::TokenizerIterator &GetTokenIter() const noexcept;
    };

    class Preprocessor final {
        std::stack<TokenizerIteratorPair> m_TokenizerStack;
        Diagnosis::Vec                   *m_pDiagnoses;
        std::unique_ptr<MacroStore> m_pMacroStore{std::make_unique<MacroStore>()
        };
        int                         m_ConditionalDepth{0};

        void SkipEmptyLines();

    public:
        Preprocessor(std::string const &filename, Diagnosis::Vec &diagnoses);

        Preprocessor(std::istream &input, Diagnosis::Vec &diagnoses);

        void OpenHeader(std::string_view filename);

        [[nodiscard]]
        PreprocessorToken GetNextFromTokenizer(bool executeCommands = true);

        [[nodiscard]]
        PreprocessorToken GetNextPreprocessorToken();

        [[nodiscard]]
        PreprocessorToken SimpleTokenRead();

        [[nodiscard]]
        MacroStore &GetMacroStore() const noexcept;

        void EmitWarning(Diagnosis &&diagnosis) const;

        template<class It = PreprocessorIterator>
            requires IsPreprocessorIterator<It> ||
                     std::same_as<It, InternalPreprocessorIterator>
        [[nodiscard]]
        It Current() {
            return It{*this};
        }

        template<class It = PreprocessorIterator>
            requires IsPreprocessorIterator<It> ||
                     std::same_as<It, InternalPreprocessorIterator>
        [[nodiscard]]
        It EndOfFile() {
            return It::end();
        }

        template<class It = PreprocessorIterator>
            requires IsPreprocessorIterator<It> ||
                     std::same_as<It, InternalPreprocessorIterator>
        [[nodiscard]]
        It Until(tokenizer::Token::Type untilType) {
            return It::Until(untilType);
        }

        template<class It = PreprocessorIterator>
            requires IsPreprocessorIterator<It> ||
                     std::same_as<It, InternalPreprocessorIterator>
        [[nodiscard]]
        It Until(std::function<bool(tokenizer::Token const &)> const
                         &untilCondition) {
            return It::Until(untilCondition);
        }

        PreprocessorIterator begin();

        PreprocessorIterator end();

        [[nodiscard]]
        Span GetCurrentSpan() const noexcept;

        [[nodiscard]]
        tokenizer::Token SkipUntilConditionEnd();

        void PushConditional();

        void PopConditional();
    };
}// namespace jcc::preprocessor

#endif// PREPROCESSOR_H
