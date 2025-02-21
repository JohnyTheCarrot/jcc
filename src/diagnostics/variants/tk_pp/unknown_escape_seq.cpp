#include "unknown_escape_seq.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    UnknownEscapeSeq::UnknownEscapeSeq(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void UnknownEscapeSeq::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
