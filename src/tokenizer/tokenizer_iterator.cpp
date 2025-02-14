#include "tokenizer_iterator.h"

#include "tokenizer.h"      // for Tokenizer
#include "tokenizer/token.h"// for Token

namespace jcc::tokenizer {
    TokenizerIterator::TokenizerIterator() = default;

    TokenizerIterator::TokenizerIterator(Tokenizer &tokenizer)
        : m_Tokenizer{&tokenizer} {
    }

    TokenizerIterator::value_type TokenizerIterator::operator*() const {
        return m_CurrentToken.value();
    }

    TokenizerIterator::pointer TokenizerIterator::operator->() const {
        return &m_CurrentToken.value();
    }

    TokenizerIterator &TokenizerIterator::operator++() {
        m_CurrentToken = m_Tokenizer->GetNextToken();

        return *this;
    }

    TokenizerIterator TokenizerIterator::operator++(int) {
        TokenizerIterator copy{*this};
        ++(*this);
        return copy;
    }

    bool TokenizerIterator::operator==(TokenizerIterator const &other) const {
        return m_CurrentToken == other.m_CurrentToken;
    }

    bool TokenizerIterator::operator!=(TokenizerIterator const &other) const {
        return !(*this == other);
    }
}// namespace jcc::tokenizer
