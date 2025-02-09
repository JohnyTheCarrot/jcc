#ifndef STATIC_TOKEN_TOKENIZER_H
#define STATIC_TOKEN_TOKENIZER_H

#include <string> // for string
#include <variant>// for variant

#include "misc/Span.h"      // for SpanMarker
#include "tokenizer/token.h"// for Token

namespace jcc::tokenizer {
    class CharIter;
}// namespace jcc::tokenizer

// By static token, it is meant that the token is a keyword or a punctuator.
// It's static because it's not input-dependent.

namespace jcc::tokenizer::static_tokens {
    struct StaticTokenTokenizationResult final {
        using ValueOrString = std::variant<Token::Value, std::string>;

        ValueOrString valueOrString{};
        std::size_t   endPos{};
    };

    [[nodiscard]]
    StaticTokenTokenizationResult
    TokenizeKeywordsAndDirectives(CharIter &charIter);

    [[nodiscard]]
    StaticTokenTokenizationResult Tokenize(CharIter &charIter);
}// namespace jcc::tokenizer::static_tokens

#endif//STATIC_TOKEN_TOKENIZER_H
