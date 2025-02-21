#include "custom_diagnostic.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    CustomDiagnostic::CustomDiagnostic(
            std::shared_ptr<Source> source, mjolnir::Span span,
            std::string message, mjolnir::BasicReportKind reportKind
    )
        : DiagnosticData{std::move(source), span.start(), reportKind}
        , m_Span{span}
        , m_Message{std::move(message)} {
    }

    void CustomDiagnostic::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
