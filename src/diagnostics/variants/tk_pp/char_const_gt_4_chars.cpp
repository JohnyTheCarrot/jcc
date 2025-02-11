#include "char_const_gt_4_chars.hpp"

namespace jcc::diagnostics {
    CharConstGt4Chars::CharConstGt4Chars(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void CharConstGt4Chars::Print(std::ostream &ostream) const {
        StartReport()
                .with_message(
                        "Character constant contains more than 4 characters."
                )
                .with_label(
                        mjolnir::Label{m_Span}
                                .with_message(
                                        "Perhaps you meant to use a string "
                                        "literal?"
                                )
                                .with_color(mjolnir::colors::light_magenta)
                )
                .with_note(
                        "Multi-character character constant values are "
                        "implementation defined."
                )
                .with_note(
                        "The value of a multi-character character constant "
                        "whose length is less than or equal to 4 is the "
                        "integer value equal to the concatenation of the "
                        "character values."
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
