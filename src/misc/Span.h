#ifndef JCC_SPAN_H
#define JCC_SPAN_H

#include <iosfwd>// for ostream, istream
#include <memory>
#include <mjolnir/span.hpp>

#include "diagnostics/source.h"

namespace jcc {
    struct Span final {
        mjolnir::Span                        m_Span;
        std::shared_ptr<diagnostics::Source> m_Source{nullptr};

        Span(std::shared_ptr<diagnostics::Source> source,
             mjolnir::Span                        span) noexcept;

        bool operator==(Span const &other) const noexcept;

        [[nodiscard]]
        Span operator+(Span const &other) const noexcept;

        [[nodiscard]]
        Span const &operator+=(Span const &other) noexcept;
    };

    void PrintTo(Span const &span, std::ostream *os);
}// namespace jcc

#endif//JCC_SPAN_H
