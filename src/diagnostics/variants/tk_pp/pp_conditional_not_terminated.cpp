#include "pp_conditional_not_terminated.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    PpConditionalNotTerminated::PpConditionalNotTerminated(
            std::shared_ptr<Source> source, mjolnir::Span conditionalSpan,
            mjolnir::Span eofSpan
    )
        : DiagnosticData{std::move(source), conditionalSpan.start(), mjolnir::BasicReportKind::Error}
        , m_ConditionalSpan{conditionalSpan}
        , m_EofSpan{eofSpan} {
    }

    void
    PpConditionalNotTerminated::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
