#ifndef STATIC_TOKEN_TOKENIZER_H
#define STATIC_TOKEN_TOKENIZER_H

#include <string>
#include <variant>

#include "misc/Span.h"
#include "tokenizer/char_iter.h"
#include "tokenizer/token.h"

// By static token, it is meant that the token is a keyword or a punctuator.
// It's static because it's not input-dependent.

namespace jcc::tokenizer::static_tokens {
    struct StaticTokenTokenizationResult final {
        using ValueOrString = std::variant<Token::Value, std::string>;

        ValueOrString valueOrString{};
        SpanMarker    endMarker{};
    };

    [[nodiscard]]
    StaticTokenTokenizationResult
    TokenizeKeywordsAndDirectives(CharIter &charIter);

    [[nodiscard]]
    StaticTokenTokenizationResult Tokenize(CharIter &charIter);
}// namespace jcc::tokenizer::static_tokens

#endif//STATIC_TOKEN_TOKENIZER_H
