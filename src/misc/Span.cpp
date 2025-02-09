#include "Span.h"

#include <mjolnir/source.hpp>
#include <ostream>// for basic_ostream, basic_ostream::operator<<
#include <utility>// for move

#include "gtest/gtest.h"// for PrintToString

namespace jcc {
    Span::Span(
            std::shared_ptr<diagnostics::Source> source, mjolnir::Span span
    ) noexcept
        : m_Span{span}
        , m_Source{std::move(source)} {
    }

    bool Span::operator==(Span const &other) const noexcept {
        return m_Span == other.m_Span && m_Source == other.m_Source;
    }

    Span Span::operator+(Span const &other) const noexcept {
        return {m_Source, m_Span + other.m_Span};
    }

    Span const &Span::operator+=(Span const &other) noexcept {
        *this = *this + other;
        return *this;
    }

    void PrintTo(Span const &span, std::ostream *os) {
        *os << '[' << span.m_Source->m_Source.get_name() << ": "
            << span.m_Span.start() << ':' << span.m_Span.end() << ']';
    }
}// namespace jcc
