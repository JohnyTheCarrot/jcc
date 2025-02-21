#include "escape_without_newline.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    EscapeWithoutNewline::EscapeWithoutNewline(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void EscapeWithoutNewline::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
