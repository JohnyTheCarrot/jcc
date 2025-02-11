#include "pp_conditional_expected_ident.hpp"

namespace jcc::diagnostics {
    PpConditionalExpectedIdent::PpConditionalExpectedIdent(
            std::shared_ptr<Source> source, mjolnir::Span condSpan,
            mjolnir::Span unexpectedTokenSpan
    )
        : DiagnosticData{std::move(source), unexpectedTokenSpan.start(), mjolnir::BasicReportKind::Error}
        , m_ConditionalSpan{condSpan}
        , m_UnexpectedTokenSpan{unexpectedTokenSpan} {
    }

    void PpConditionalExpectedIdent::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Preprocessor conditional expected identifier.")
                .with_label(
                        mjolnir::Label{m_ConditionalSpan}.with_color(
                                mjolnir::colors::light_cyan
                        )
                )
                .with_label(
                        mjolnir::Label{m_UnexpectedTokenSpan}
                                .with_message(
                                        "Unexpected token, expected identifier"
                                )
                                .with_color(mjolnir::colors::light_red)
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
