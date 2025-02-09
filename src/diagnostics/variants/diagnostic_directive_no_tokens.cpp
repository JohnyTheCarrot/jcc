#include "diagnostic_directive_no_tokens.hpp"

namespace jcc::diagnostics {
    DiagnosticDirectiveNoTokens::DiagnosticDirectiveNoTokens(
            std::shared_ptr<Source> source, DiagnosticDirectiveKind kind,
            mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span}
        , m_Kind{kind} {
    }

    void DiagnosticDirectiveNoTokens::Print(std::ostream &ostream) const {
        mjolnir::Report report{StartReport()};

        if (m_Kind == DiagnosticDirectiveKind::Error) {
            report.with_message("Error directive without tokens");
        } else {
            report.with_message("Warning directive without tokens");
        }

        report.with_label(
                      mjolnir::Label{m_Span}.with_color(
                              mjolnir::colors::light_red
                      )
        )
                .print(ostream);
    }
}// namespace jcc::diagnostics
