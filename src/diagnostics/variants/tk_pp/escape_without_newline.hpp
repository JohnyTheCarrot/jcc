#ifndef ESCAPE_WITHOUT_NEWLINE_HPP
#define ESCAPE_WITHOUT_NEWLINE_HPP

#include <diagnostics/diagnostics.h>
#include <mjolnir/span.hpp>

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
