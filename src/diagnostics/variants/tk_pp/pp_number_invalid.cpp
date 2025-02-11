#include "pp_number_invalid.hpp"

namespace jcc::diagnostics {
    PpNumberInvalid::PpNumberInvalid(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void PpNumberInvalid::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Invalid preprocessing number.")
                .with_label(
                        mjolnir::Label{m_Span}.with_color(
                                mjolnir::colors::light_red
                        )
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
