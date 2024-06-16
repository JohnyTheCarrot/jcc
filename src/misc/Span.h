//
// Created by Tuur Martens on 15/01/2024.
//

#ifndef JCC_SPAN_H
#define JCC_SPAN_H

#include "config.h"
#include <fmt/core.h>
#include <gtest/gtest-printers.h>

namespace jcc {
	struct SpanMarker final {
		static constexpr int FIRST_LINE{1}, FIRST_CHAR{1};

		int m_LineNumber{FIRST_LINE};
		// FIRST_CHAR is reduced by one because the initial position of the stream is one before the first char
		int m_CharacterIndex{FIRST_CHAR - 1};
		int m_RealCharacterIndex{-1};

		void NextChar(bool shouldIncrementReal = true) noexcept;

		void NextLine() noexcept;

		bool operator==(SpanMarker const &other) const noexcept;

		friend void PrintTo(SpanMarker const &spanMarker, std::ostream *os) {
			*os << spanMarker.m_LineNumber << ':' << spanMarker.m_CharacterIndex;
		}

		friend std::ostream &operator<<(std::ostream &os, SpanMarker const &spanMarker) {
			os << spanMarker.m_LineNumber << ':' << spanMarker.m_CharacterIndex;
			return os;
		}
	};

	struct Span final {
		std::shared_ptr<std::string> m_FileName;
		SpanMarker                   m_Start{}, m_End{};
		std::streampos               m_StartPos{};
		std::istream                *m_IStream;

		Span(std::shared_ptr<std::string> const &fileName, SpanMarker const &start, SpanMarker const &end,
		     std::streampos const &startPos, std::istream *inputStream) noexcept
		    : m_FileName{fileName}
		    , m_Start{start}
		    , m_End{end}
		    , m_StartPos{startPos}
		    , m_IStream{inputStream} {
		}

		bool operator==(Span const &other) const noexcept;

		friend void PrintTo(Span const &span, std::ostream *os) {
			*os << testing::PrintToString(span.m_Start) << " - " << testing::PrintToString(span.m_End);
		}
	};
}// namespace jcc

#endif//JCC_SPAN_H
