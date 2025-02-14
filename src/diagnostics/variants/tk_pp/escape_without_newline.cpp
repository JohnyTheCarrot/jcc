#include "escape_without_newline.hpp"

namespace jcc::diagnostics {
    EscapeWithoutNewline::EscapeWithoutNewline(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void EscapeWithoutNewline::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Newline escape sequence without a newline.")
                .with_label(
                        mjolnir::Label{m_Span}.with_color(
                                mjolnir::colors::light_red
                        )
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
