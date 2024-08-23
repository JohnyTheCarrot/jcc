#ifndef CHARACTER_CONSTANTS_H
#define CHARACTER_CONSTANTS_H

#include "tokenizer/char_iter.h"
#include "tokenizer/token.h"

namespace jcc::tokenizer::character_constants {
	[[nodiscard]]
	// The caller is expected to have already consumed the opening single quote.
	Token Tokenize(CharIter &charIter, ConstantPrefix prefix, SpanMarker const &startMarker);
}

#endif//CHARACTER_CONSTANTS_H
