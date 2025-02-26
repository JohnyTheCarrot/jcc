#include "directive_expected_newline.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    DirectiveExpectedNewline::DirectiveExpectedNewline(
            std::shared_ptr<Source> source, mjolnir::Span directiveSpan,
            mjolnir::Span nextTokenSpan
    )
        : DiagnosticData{std::move(source), nextTokenSpan.start(), mjolnir::BasicReportKind::Error}
        , m_DirectiveSpan{directiveSpan}
        , m_NextTokenSpan{nextTokenSpan} {
    }

    void
    DirectiveExpectedNewline::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
