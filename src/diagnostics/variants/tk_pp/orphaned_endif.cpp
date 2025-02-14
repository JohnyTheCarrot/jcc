#include "orphaned_endif.hpp"

namespace jcc::diagnostics {
    OrphanedEndif::OrphanedEndif(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void OrphanedEndif::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Orphaned #endif directive.")
                .with_label(
                        mjolnir::Label{m_Span}.with_color(
                                mjolnir::colors::light_red
                        )
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
