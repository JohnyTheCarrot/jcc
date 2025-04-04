#include "include_open_failed.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    IncludeOpenFailed::IncludeOpenFailed(
            std::shared_ptr<Source> source, mjolnir::Span directiveSpan,
            mjolnir::Span headerNameSpan
    )
        : DiagnosticData{std::move(source), headerNameSpan.start(), mjolnir::BasicReportKind::Error}
        , m_DirectiveSpan{directiveSpan}
        , m_HeaderNameSpan{headerNameSpan} {
    }

    void IncludeOpenFailed::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
