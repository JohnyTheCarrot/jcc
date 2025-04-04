#ifndef JCC_SPAN_H
#define JCC_SPAN_H

#include <iosfwd>          // for ostream
#include <memory>          // for shared_ptr
#include <mjolnir/span.hpp>// for Span

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics

    struct Span final {
        mjolnir::Span                        m_Span;
        std::shared_ptr<diagnostics::Source> m_Source{nullptr};

        Span(std::shared_ptr<diagnostics::Source> source,
             mjolnir::Span                        span) noexcept;

        bool operator==(Span const &other) const noexcept;

        [[nodiscard]]
        Span operator+(Span const &other) const noexcept;

        Span const &operator+=(Span const &other) noexcept;
    };

    void PrintTo(Span const &span, std::ostream *os);
}// namespace jcc

#endif//JCC_SPAN_H
