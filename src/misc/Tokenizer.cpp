//
// Created by Tuur Martens on 12/01/2024.
//

#include "Tokenizer.h"

void Tokenizer::Tokenize(TokenList &tokens) {
	std::optional<TokenType> tokenType;
	std::size_t lineCharacterStartIndex, lineCharacterEndIndex{FIRST_CHAR_INDEX};

	while (m_IStream.good()) {
		lineCharacterStartIndex = lineCharacterEndIndex;

		Char whitespace{};
		if (std::isspace(whitespace = m_IStream.peek()) && whitespace != '\r' && whitespace != '\n') {
			m_IStream.get();
			++lineCharacterEndIndex;
			continue;
		}

		std::size_t foundTokenLength{};
		tokenType = m_TokenDefs.Find(m_IStream, foundTokenLength);
		lineCharacterEndIndex += foundTokenLength;

		if (tokenType) {
			switch (*tokenType) {
				case TokenType::Util_NewLine:
					++m_LineNumber;
					lineCharacterEndIndex = FIRST_CHAR_INDEX;
					continue;
				// TODO: this isn't part of the tokenizer, it should be handled by the preprocessor
				case TokenType::Util_LineComment:
					while (m_IStream.peek() != EOFChar && m_IStream.peek() != C('\n')) {
						m_IStream.get();
						++lineCharacterEndIndex;
					}
					continue;
				default:
					break;
			}

			const Span span{
					.m_LineNumberStart	   = m_LineNumber,
					.m_LineNumberEnd	   = m_LineNumber,
					.m_CharacterIndexStart = lineCharacterStartIndex,
					.m_CharacterIndexEnd   = lineCharacterEndIndex
			};

			tokens.emplace_back(*tokenType, span, std::nullopt);
			continue;
		}

		// TODO: check for numbers first, otherwise identifiers starting with a number will be parsed as a number

		String identifier{};
		Char c;
		while ((c = m_IStream.peek()) != EOFChar && std::isalnum(c) || c == C('_')) {
			++lineCharacterEndIndex;
			identifier += c;
			m_IStream.get();
		}
	}
}
