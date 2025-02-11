#include "unknown_escape_seq.hpp"

namespace jcc::diagnostics {
    UnknownEscapeSeq::UnknownEscapeSeq(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void UnknownEscapeSeq::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Unknown escape sequence.")
                .with_label(
                        mjolnir::Label{m_Span}.with_color(
                                mjolnir::colors::light_red
                        )
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
