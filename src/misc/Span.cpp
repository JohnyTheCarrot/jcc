#include "Span.h"

namespace jcc {
    bool SpanMarker::operator==(SpanMarker const &other) const noexcept {
        return m_LineNumber == other.m_LineNumber &&
               m_CharacterIndex == other.m_CharacterIndex &&
               m_RealCharacterIndex == other.m_RealCharacterIndex;
    }

    void SpanMarker::NextChar(bool shouldIncrementReal, int number) noexcept {
        m_CharacterIndex += number;
        if (shouldIncrementReal)
            m_RealCharacterIndex += number;
    }

    void SpanMarker::NextLine() noexcept {
        ++m_LineNumber;
        m_CharacterIndex     = FIRST_CHAR - 1;
        m_RealCharacterIndex = FIRST_CHAR - 2;
    }

    Span::Span(
            std::shared_ptr<std::string> fileName, SpanMarker const &start,
            SpanMarker const &end, std::istream *inputStream
    ) noexcept
        : m_FileName{std::move(fileName)}
        , m_Start{start}
        , m_End{end}
        , m_IStream{inputStream} {
    }

    bool Span::operator==(Span const &other) const noexcept {
        return m_Start == other.m_Start && m_End == other.m_End;
    }

    Span Span::operator+(Span const &other) const noexcept {
        return {m_FileName, m_Start, other.m_End, m_IStream};
    }

    Span const &Span::operator+=(Span const &other) noexcept {
        *this = *this + other;
        return *this;
    }

    void PrintTo(SpanMarker const &spanMarker, std::ostream *os) {
        *os << spanMarker.m_LineNumber << ':' << spanMarker.m_CharacterIndex
            << " (real: " << spanMarker.m_RealCharacterIndex << ')';
    }

    std::ostream &operator<<(std::ostream &os, SpanMarker const &spanMarker) {
        os << spanMarker.m_LineNumber << ':' << spanMarker.m_CharacterIndex;
        return os;
    }

    void PrintTo(Span const &span, std::ostream *os) {
        *os << testing::PrintToString(span.m_Start) << " - "
            << testing::PrintToString(span.m_End);
    }
}// namespace jcc
