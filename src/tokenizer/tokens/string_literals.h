#ifndef STRING_LITERALS_H
#define STRING_LITERALS_H

#include "tokenizer/token.h"

namespace jcc::tokenizer {
	class CharIter;
}

namespace jcc::tokenizer::string_literals {
	[[nodiscard]]
	// The caller is expected to have already consumed the opening double quote.
	Token Tokenize(CharIter &charIter, ConstantPrefix prefix, SpanMarker const &startMarker);
}

#endif//STRING_LITERALS_H
