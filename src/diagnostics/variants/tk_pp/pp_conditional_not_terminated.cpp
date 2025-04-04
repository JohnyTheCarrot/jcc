#include "pp_conditional_not_terminated.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...
#include "mjolnir/span.hpp"                                     // for Span

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    PpConditionalNotTerminated::PpConditionalNotTerminated(
            std::shared_ptr<Source> source, mjolnir::Span conditionalSpan,
            mjolnir::Span eofSpan
    )
        : DiagnosticData{std::move(source), conditionalSpan.start(), mjolnir::BasicReportKind::Error}
        , m_ConditionalSpan{conditionalSpan}
        , m_EofSpan{eofSpan} {
    }

    void PpConditionalNotTerminated::Visit(DiagnosticsVisitor const &visitor
    ) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
