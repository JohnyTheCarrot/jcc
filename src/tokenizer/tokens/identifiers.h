#ifndef IDENTIFIER_TOKENIZER_H
#define IDENTIFIER_TOKENIZER_H

#include "tokenizer/char_iter.h"
#include "tokenizer/token.h"

namespace jcc::tokenizer::identifiers {
	[[nodiscard]]
	std::optional<SpanMarker> CollectRestOfIdentifier(CharIter &charIter, std::string &identifier);

	struct IdentifierTokenStart final {
		std::string    m_Identifier{};
		SpanMarker     m_Start{};
		std::streampos m_StartPos{};
	};

	[[nodiscard]]
	Token Tokenize(CharIter const &charIter, IdentifierTokenStart const &tokenStart);
}// namespace jcc::tokenizer::identifiers

#endif//IDENTIFIER_TOKENIZER_H
