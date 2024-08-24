#ifndef PP_NUMBER_H
#define PP_NUMBER_H

#include "tokenizer/token.h"

namespace jcc {
	struct SpanMarker;
}

namespace jcc::tokenizer {
	class CharIter;
}

namespace jcc::tokenizer::pp_numbers {
	[[nodiscard]]
	Token Tokenize(CharIter &charIter, SpanMarker const &start, bool startsWithDot);
}

#endif//PP_NUMBER_H
