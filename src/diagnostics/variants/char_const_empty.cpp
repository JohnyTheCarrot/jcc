#include "char_const_empty.h"

namespace jcc::diagnostics {
    CharConstEmpty::CharConstEmpty(
            std::shared_ptr<Source> const &source, mjolnir::Span span,
            std::optional<mjolnir::Span> potentiallyClosingQuote
    )
        : DiagnosticData{source, span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span}
        , m_PotentiallyClosingQuote{potentiallyClosingQuote} {
    }

    void CharConstEmpty::Print(std::ostream &ostream) const {
        mjolnir::Report report{StartReport()};

        report.with_message("Character constant is empty.");

        if (m_PotentiallyClosingQuote.has_value()) {
            report.with_label(
                          mjolnir::Label{m_Span}
                                  .with_color(mjolnir::colors::light_red)
                                  .with_message(
                                          "This is the effective character "
                                          "constant, which is empty."
                                  )
            )
                    .with_label(
                            mjolnir::Label{m_PotentiallyClosingQuote.value()}
                                    .with_message(
                                            "This looks like it was meant to "
                                            "be "
                                            "the closing quote."
                                    )
                                    .with_color(mjolnir::colors::light_green)
                    )
                    .with_help("Perhaps you meant to write '\\''?");
        } else {
            report.with_label(
                    mjolnir::Label{m_Span}
                            .with_color(mjolnir::colors::light_red)
                            .with_message("This character constant is empty.")
            );
        }

        report.print(ostream);
    }
}// namespace jcc::diagnostics
