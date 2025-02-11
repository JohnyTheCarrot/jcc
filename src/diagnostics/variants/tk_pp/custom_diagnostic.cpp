#include "custom_diagnostic.hpp"

namespace jcc::diagnostics {
    CustomDiagnostic::CustomDiagnostic(
            std::shared_ptr<Source> source, mjolnir::Span span,
            std::string message, mjolnir::BasicReportKind reportKind
    )
        : DiagnosticData{std::move(source), span.start(), reportKind}
        , m_Span{span}
        , m_Message{std::move(message)} {
    }

    void CustomDiagnostic::Print(std::ostream &ostream) const {
        StartReport()
                .with_message(m_Message)
                .with_label(mjolnir::Label{m_Span})
                .print(ostream);
    }
}// namespace jcc::diagnostics
