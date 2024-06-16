//
// Created by Tuur Martens on 15/01/2024.
//

#include "Span.h"

namespace jcc {
	bool SpanMarker::operator==(SpanMarker const &other) const noexcept {
		return m_LineNumber == other.m_LineNumber && m_CharacterIndex == other.m_CharacterIndex;
	}

	void SpanMarker::NextChar(bool shouldIncrementReal) noexcept {
		++m_CharacterIndex;
		if (shouldIncrementReal)
			++m_RealCharacterIndex;
	}

	void SpanMarker::NextLine() noexcept {
		++m_LineNumber;
		m_CharacterIndex     = FIRST_CHAR - 1;
		m_RealCharacterIndex = FIRST_CHAR - 2;
	}

	bool Span::operator==(Span const &other) const noexcept {
		return m_Start == other.m_Start && m_End == other.m_End;
	}
}// namespace jcc
