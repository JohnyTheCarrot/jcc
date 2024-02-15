//
// Created by Tuur Martens on 15/01/2024.
//

#ifndef JCC_SPAN_H
#define JCC_SPAN_H
#include "config.h"
#include <fmt/core.h>
#include <gtest/gtest-printers.h>

struct SpanMarker final {
	static constexpr int FIRST_LINE{1}, FIRST_CHAR{1};

	int m_LineNumber{FIRST_LINE};
	// FIRST_CHAR is reduced by one because the initial position of the stream is one before the first char
	int m_CharacterIndex{FIRST_CHAR - 1};
	int m_RealCharacterIndex{-1};

	void NextChar(bool shouldIncrementReal = true) noexcept;

	void NextLine() noexcept;

	bool operator==(const SpanMarker &other) const noexcept;

	friend void PrintTo(const SpanMarker &spanMarker, std::ostream *os) {
		*os << spanMarker.m_LineNumber << ':' << spanMarker.m_CharacterIndex;
	}

	friend std::ostream &operator<<(std::ostream &os, const SpanMarker &spanMarker) {
		os << spanMarker.m_LineNumber << ':' << spanMarker.m_CharacterIndex;
		return os;
	}
};

struct Span final {
	std::shared_ptr<std::string> m_FileName;
	SpanMarker                   m_Start{}, m_End{};
	std::streampos               m_StartPos{};
	std::istream                *m_IStream;

	Span(const std::shared_ptr<std::string> &fileName, const SpanMarker &start, const SpanMarker &end,
	     const std::streampos &startPos, std::istream *inputStream) noexcept
	    : m_FileName{fileName}
	    , m_Start{start}
	    , m_End{end}
	    , m_StartPos{startPos}
	    , m_IStream{inputStream} {
	}

	bool operator==(const Span &other) const noexcept;

	friend void PrintTo(const Span &span, std::ostream *os) {
		*os << testing::PrintToString(span.m_Start) << " - " << testing::PrintToString(span.m_End);
	}
};

#endif//JCC_SPAN_H
