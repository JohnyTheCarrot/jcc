#include "pp_conditional_not_terminated.hpp"

namespace jcc::diagnostics {

    PpConditionalNotTerminated::PpConditionalNotTerminated(
            std::shared_ptr<Source> source, mjolnir::Span conditionalSpan,
            mjolnir::Span eofSpan
    )
        : DiagnosticData{std::move(source), conditionalSpan.start(), mjolnir::BasicReportKind::Error}
        , m_ConditionalSpan{conditionalSpan}
        , m_EofSpan{eofSpan} {
    }

    void PpConditionalNotTerminated::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Preprocessor conditional was not terminated")
                .with_label(
                        mjolnir::Label{m_ConditionalSpan}
                                .with_message("Conditional starts here")
                                .with_color(mjolnir::colors::light_cyan)
                )
                .with_label(
                        mjolnir::Label{m_EofSpan}
                                .with_message("End of file")
                                .with_color(mjolnir::colors::light_red)
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
