#include "multi_byte_char_impl_def.hpp"

namespace jcc::diagnostics {
    MultiByteCharImplDef::MultiByteCharImplDef(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Warning}
        , m_Span{span} {
    }

    void MultiByteCharImplDef::Print(std::ostream &ostream) const {
        StartReport()
                .with_message(
                        "The values of multi-character character constants "
                        "are implementation-defined."
                )
                .with_note(
                        "This implementation defines the behavior to be an "
                        "integer value "
                        "equal to the concatenation of the character values."
                )
                .with_label(
                        mjolnir::Label{m_Span}
                                .with_color(mjolnir::colors::light_magenta)
                                .with_message(
                                        "Perhaps you meant to use a string "
                                        "literal?"
                                )
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
