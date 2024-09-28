#ifndef JCC_TOKENIZER_H
#define JCC_TOKENIZER_H

#include <optional>
#include <string>
#include <string_view>

#include "char_iter.h"
#include "tokenizer/token.h"
#include "tokenizer_iterator.h"
#include "tokens/static_tokens.h"

namespace jcc::tokenizer {
    class Tokenizer final {
        CharIter m_CharIter;

        struct TrieTraversalResult final {
            std::variant<Token::Value, std::string> valueOrString{};
            SpanMarker                              endMarker{};
        };

        [[nodiscard]]
        bool SkipWhitespace();

        void SkipLineComment();

        void SkipBlockComment(Span &span);

        [[nodiscard]]
        std::optional<Token> TokenizeStaticToken(
                static_tokens::StaticTokenTokenizationResult::
                        ValueOrString const &valueOrString,
                Span &span, bool skippedWhitespace
        );

    public:
        explicit Tokenizer(std::istream &input, std::string_view fileName);

        [[nodiscard]]
        std::optional<Token> GetNextToken();

        [[nodiscard]]
        TokenizerIterator begin();

        [[nodiscard]]
        TokenizerIterator end();

        [[nodiscard]]
        Span GetLastSpan() const;
    };
}// namespace jcc::tokenizer

#endif//JCC_TOKENIZER_H
