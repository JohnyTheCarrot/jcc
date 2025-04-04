#include "preprocessor_iterator.h"

#include <utility>// for move

#include "preprocessor.h"                   // for Preprocessor
#include "preprocessor/preprocessor_token.h"// for PreprocessorToken

namespace jcc::preprocessor {
    tokenizer::Token PreprocessorIterator::GetNextToken() const {
        auto token{m_pPreprocessor->GetNextPreprocessorToken().m_Token};
        if (token.Is(tokenizer::Punctuator::PpLeftParenthesis))
            token.m_Value = tokenizer::Punctuator::LeftParenthesis;

        return token;
    }

    tokenizer::Token PreprocessorIteratorNoCommands::GetNextToken() const {
        return m_pPreprocessor->SimpleTokenRead().m_Token;
    }

    InternalPreprocessorIterator::InternalPreprocessorIterator(
            Preprocessor &preprocessor
    )
        : m_pPreprocessor{&preprocessor} {
    }

    InternalPreprocessorIterator::InternalPreprocessorIterator(
            tokenizer::Token::Type untilType
    )
        : m_Token{untilType} {
    }

    InternalPreprocessorIterator
    InternalPreprocessorIterator::Until(tokenizer::Token::Type untilType) {
        return InternalPreprocessorIterator{untilType};
    }

    InternalPreprocessorIterator::reference
    InternalPreprocessorIterator::operator*() const {
        return std::get<PreprocessorToken>(m_Token);
    }

    InternalPreprocessorIterator::pointer
    InternalPreprocessorIterator::operator->() const {
        return &std::get<PreprocessorToken>(m_Token);
    }

    InternalPreprocessorIterator &InternalPreprocessorIterator::operator++() {
        auto ppToken{m_pPreprocessor->GetNextFromTokenizer(true)};

        if (auto const &[token, isFromMacro]{ppToken};
            token.Is(tokenizer::SpecialPurpose::EndOfFile) ||
            (std::holds_alternative<tokenizer::Token::Type>(m_Token) &&
             std::get<tokenizer::Token::Type>(m_Token) == token.GetValueType()))
            m_Token = token.GetValueType();
        else
            m_Token = std::move(ppToken);

        return *this;
    }

    InternalPreprocessorIterator InternalPreprocessorIterator::operator++(int) {
        InternalPreprocessorIterator tmp{*this};
        ++(*this);
        return tmp;
    }

    InternalPreprocessorIterator InternalPreprocessorIterator::begin() const {
        return InternalPreprocessorIterator{*m_pPreprocessor};
    }

    InternalPreprocessorIterator InternalPreprocessorIterator::end() {
        return InternalPreprocessorIterator{};
    }

    bool InternalPreprocessorIterator::operator==(
            InternalPreprocessorIterator const &other
    ) const noexcept {
        return m_Token == other.m_Token;
    }

    bool InternalPreprocessorIterator::operator!=(
            InternalPreprocessorIterator const &other
    ) const noexcept {
        return !(*this == other);
    }

    InternalPreprocessorIterator::InternalPreprocessorIterator() = default;
}// namespace jcc::preprocessor
