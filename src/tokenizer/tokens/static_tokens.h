#ifndef STATIC_TOKEN_TOKENIZER_H
#define STATIC_TOKEN_TOKENIZER_H

#include "misc/Span.h"
#include "tokenizer/char_iter.h"
#include "tokenizer/token.h"
#include <string>
#include <variant>

// By static token, it is meant that the token is a keyword or a punctuator.
// It's static because it's not input-dependent.

namespace jcc::tokenizer::static_tokens {
	struct StaticTokenTokenizationResult final {
		std::variant<Token::Value, std::string> valueOrString{};
		SpanMarker                              endMarker{};
	};

	[[nodiscard]]
	StaticTokenTokenizationResult Tokenize(CharIter &charIter);
}// namespace jcc::tokenizer::static_tokens

#endif//STATIC_TOKEN_TOKENIZER_H
