#ifndef UTF8_TOO_MANY_CHARS_HPP
#define UTF8_TOO_MANY_CHARS_HPP

#include <memory>          // for shared_ptr
#include <mjolnir/span.hpp>// for Span

#include "diagnostics/diagnostics.h"// for DiagnosticData

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    struct Utf8TooManyChars final : DiagnosticData {
        mjolnir::Span m_Span;

        Utf8TooManyChars(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UTF8_TOO_MANY_CHARS_HPP
