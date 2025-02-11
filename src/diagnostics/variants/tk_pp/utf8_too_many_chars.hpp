#ifndef UTF8_TOO_MANY_CHARS_HPP
#define UTF8_TOO_MANY_CHARS_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class Utf8TooManyChars final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        Utf8TooManyChars(
                std::shared_ptr<Source> const &source, mjolnir::Span span
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//UTF8_TOO_MANY_CHARS_HPP
