#include "include_expected_header_name.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    IncludeExpectedHeaderName::IncludeExpectedHeaderName(
            std::shared_ptr<Source> source, mjolnir::Span directiveSpan,
            mjolnir::Span headerNameSpan
    )
        : DiagnosticData{std::move(source), directiveSpan.start(), mjolnir::BasicReportKind::Error}
        , m_DirectiveSpan{directiveSpan}
        , m_HeaderNameSpan{headerNameSpan} {
    }

    void
    IncludeExpectedHeaderName::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
