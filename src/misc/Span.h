#ifndef JCC_SPAN_H
#define JCC_SPAN_H

#include <memory>
#include <string>

#include <gtest/gtest-printers.h>

namespace jcc {
    struct SpanMarker final {
        static constexpr int FIRST_LINE{1}, FIRST_CHAR{1};

        int m_LineNumber{FIRST_LINE};
        int m_CharacterIndex{FIRST_CHAR};
        int m_RealCharacterIndex{0};

        void NextChar(bool shouldIncrementReal = true, int number = 1) noexcept;

        void NextLine() noexcept;

        bool operator==(SpanMarker const &other) const noexcept;

        friend void PrintTo(SpanMarker const &spanMarker, std::ostream *os);

        friend std::ostream &
        operator<<(std::ostream &os, SpanMarker const &spanMarker);
    };

    struct Span final {
        std::shared_ptr<std::string> m_FileName;
        SpanMarker                   m_Start{}, m_End{};
        std::istream                *m_IStream;

        Span(std::shared_ptr<std::string> fileName, SpanMarker const &start,
             SpanMarker const &end, std::istream *inputStream) noexcept;

        bool operator==(Span const &other) const noexcept;

        friend void PrintTo(Span const &span, std::ostream *os);

        [[nodiscard]]
        Span
        operator+(Span const &other) const noexcept;

        [[nodiscard]]
        Span const &
        operator+=(Span const &other) noexcept;
    };
}// namespace jcc

#endif//JCC_SPAN_H
