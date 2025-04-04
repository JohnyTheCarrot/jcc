#include "todo.hpp"

#include <utility>// for move

#include "mjolnir/report.hpp"              // for BasicReportKind
#include "mjolnir/span.hpp"                // for Span
#include "visitors/diagnostics_visitor.hpp"// for DiagnosticsVisitor

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    TodoError::TodoError(std::shared_ptr<Source> source, mjolnir::Span span)
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void TodoError::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
