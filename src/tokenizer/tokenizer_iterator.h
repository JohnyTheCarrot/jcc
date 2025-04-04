#ifndef TOKENIZER_ITERATOR_H
#define TOKENIZER_ITERATOR_H

#include <iterator>// for input_iterator, input_iterator_tag
#include <optional>// for optional

#include "token.h"// for Token

namespace jcc::tokenizer {
    class Tokenizer;

    class TokenizerIterator final {
        std::optional<Token> m_CurrentToken{};
        Tokenizer           *m_Tokenizer{};

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = Token;
        using pointer           = Token const *;
        using difference_type   = int;

        TokenizerIterator();

        explicit TokenizerIterator(Tokenizer &tokenizer);

        [[nodiscard]]
        value_type operator*() const;

        [[nodiscard]]
        pointer operator->() const;

        TokenizerIterator &operator++();

        [[nodiscard]]
        TokenizerIterator operator++(int);

        [[nodiscard]]
        bool operator==(TokenizerIterator const &other) const;

        [[nodiscard]]
        bool operator!=(TokenizerIterator const &other) const;
    };

    static_assert(std::input_iterator<TokenizerIterator>);
}// namespace jcc::tokenizer

#endif//TOKENIZER_ITERATOR_H
