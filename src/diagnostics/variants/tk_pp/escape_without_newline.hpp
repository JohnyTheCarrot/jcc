#ifndef ESCAPE_WITHOUT_NEWLINE_HPP
#define ESCAPE_WITHOUT_NEWLINE_HPP

#include <diagnostics/diagnostics.h>// for DiagnosticData
#include <memory>                   // for shared_ptr
#include <mjolnir/span.hpp>         // for Span

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    struct EscapeWithoutNewline final : DiagnosticData {
        mjolnir::Span m_Span;

        EscapeWithoutNewline(
                std::shared_ptr<Source> source, mjolnir::Span span
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//ESCAPE_WITHOUT_NEWLINE_HPP
