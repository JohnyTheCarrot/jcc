#ifndef IDENTIFIER_TOKENIZER_H
#define IDENTIFIER_TOKENIZER_H

#include "tokenizer/char_iter.h"
#include "tokenizer/token.h"

namespace jcc::tokenizer::identifiers {
	struct IdentifierTokenStart final {
		std::string    m_Identifier{};
		SpanMarker     m_Start{};
		std::streampos m_StartPos{};
	};

	[[nodiscard]]
	Token Tokenize(CharIter &charIter, IdentifierTokenStart const &tokenStart);
}// namespace jcc::tokenizer::identifier_tokenizer

#endif//IDENTIFIER_TOKENIZER_H
