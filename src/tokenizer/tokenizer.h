#ifndef JCC_TOKENIZER_H
#define JCC_TOKENIZER_H

#include <fstream>
#include <optional>
#include <string>

#include "char_iter.h"
#include "tokenizer/token.h"
#include "tokenizer_iterator.h"
#include "tokens/static_tokens.h"

namespace jcc::tokenizer {
    class TokenizerFileOpenFailure final : public std::exception {};

    class Tokenizer final {
        // Dependent on whether we own the input stream or not.
        using InputPtr =
                std::variant<std::unique_ptr<std::istream>, std::istream *>;

        InputPtr m_Input;
        CharIter m_CharIter;

        [[nodiscard]]
        std::istream &GetStream() const;

        struct TrieTraversalResult final {
            std::variant<Token::Value, std::string> valueOrString{};
            SpanMarker                              endMarker{};
        };

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
        explicit Tokenizer(std::string const &fileName);

        explicit Tokenizer(std::istream &input);

        Tokenizer(Tokenizer const &) = delete;

        Tokenizer(Tokenizer &&) noexcept;

        Tokenizer &operator=(Tokenizer const &) = delete;

        Tokenizer &operator=(Tokenizer &&) noexcept;

        [[nodiscard]]
        std::optional<Token> GetNextToken();

        [[nodiscard]]
        TokenizerIterator begin();

        [[nodiscard]]
        TokenizerIterator end() const;

        [[nodiscard]]
        Span GetLastSpan() const;

        [[nodiscard]]
        Token SkipUntilConditionEnd();
    };
}// namespace jcc::tokenizer

#endif//JCC_TOKENIZER_H
