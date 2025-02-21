#include "escape_seq_too_large.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    EscapeSeqTooLarge::EscapeSeqTooLarge(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void EscapeSeqTooLarge::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
