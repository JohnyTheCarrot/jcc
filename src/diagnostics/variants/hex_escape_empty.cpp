#include "hex_escape_empty.hpp"

namespace jcc::diagnostics {
    HexEscapeEmpty::HexEscapeEmpty(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void HexEscapeEmpty::Print(std::ostream &ostream) const {
        StartReport().with_message("Hexadecimal escape sequence is empty")
                .with_label(
                        mjolnir::Label{m_Span}.with_color(
                                mjolnir::colors::light_red
                        )
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
