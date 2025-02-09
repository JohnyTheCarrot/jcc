#include "untermed_string.h"

namespace jcc::diagnostics {
    UntermedString::UntermedString(
            std::shared_ptr<Source> const &source, mjolnir::Span span
    )
        : DiagnosticData{source, span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void UntermedString::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Unterminated string constant.")
                .with_label(
                        mjolnir::Label{{m_Span.end() - 1, m_Span.end()}}
                                .with_message("Reached end of line")
                                .with_color(mjolnir::colors::light_red)
                )
                .with_help(
                        "If you intended to write a multi-line string, "
                        "consider "
                        "escaping the newline character."
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
