#include "escape_seq_too_large.hpp"

namespace jcc::diagnostics {
    EscapeSeqTooLarge::EscapeSeqTooLarge(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void EscapeSeqTooLarge::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Escape sequence value is too large")
                .with_label(
                        mjolnir::Label{m_Span}.with_color(
                                mjolnir::colors::light_red
                        )
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
