#include "utf8_too_many_chars.hpp"

namespace jcc::diagnostics {
    Utf8TooManyChars::Utf8TooManyChars(
            std::shared_ptr<Source> const &source, mjolnir::Span span
    )
        : DiagnosticData{source, span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void Utf8TooManyChars::Print(std::ostream &ostream) const {
        StartReport()
                .with_message(
                        "A UTF-8, UTF-16, or UTF-32 character literal must "
                        "contain exactly one character."
                )
                .with_label(
                        mjolnir::Label{m_Span}.with_color(
                                mjolnir::colors::light_red
                        )
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
