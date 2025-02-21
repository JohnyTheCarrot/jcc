#include "diagnostic_directive_no_tokens.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    DiagnosticDirectiveNoTokens::DiagnosticDirectiveNoTokens(
            std::shared_ptr<Source> source, DiagnosticDirectiveKind kind,
            mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span}
        , m_Kind{kind} {
    }

    void DiagnosticDirectiveNoTokens::Visit(
            DiagnosticsVisitor const &visitor
    ) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
