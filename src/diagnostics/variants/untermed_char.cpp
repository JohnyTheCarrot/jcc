#include "untermed_char.h"

namespace jcc::diagnostics {
    UntermedChar::UntermedChar(
            std::shared_ptr<Source> const &source, mjolnir::Span span
    )
        : DiagnosticData{source, span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void UntermedChar::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Unterminated character constant.")
                .with_label(
                        mjolnir::Label{{m_Span.end() - 1, m_Span.end()}}
                                .with_message("Reached end of line")
                                .with_color(mjolnir::colors::light_red)
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
