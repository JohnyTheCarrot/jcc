#include "illegal_macro_redef.hpp"

namespace jcc::diagnostics {
    IllegalMacroRedef::IllegalMacroRedef(Span definedLoc, Span redefineLoc)
        : DiagnosticData{redefineLoc.m_Source, redefineLoc.m_Span.start(), mjolnir::BasicReportKind::Error}
        , m_DefSpan{std::move(definedLoc)}
        , m_RedefineSpan{std::move(redefineLoc)} {
    }

    void IllegalMacroRedef::Print(std::ostream &ostream) const {
        std::string                   message{"A macro may not be redefined."};
        std::optional<mjolnir::Label> defLabel{};
        mjolnir::BasicReportKind      kind{mjolnir::BasicReportKind::Error};

        if (m_DefSpan.m_Source == m_RedefineSpan.m_Source) {
            defLabel = mjolnir::Label{m_DefSpan.m_Span}
                               .with_message("First defined here")
                               .with_color(mjolnir::colors::light_cyan);
        } else {
            kind = mjolnir::BasicReportKind::Continuation;
            mjolnir::Report{
                    mjolnir::BasicReportKind::Error,
                    m_DefSpan.m_Source->m_Source, m_DefSpan.m_Span.start()
            }
                    .with_message(std::move(message))
                    .with_label(
                            mjolnir::Label{m_DefSpan.m_Span}
                                    .with_message("First defined here")
                                    .with_color(mjolnir::colors::light_cyan)
                    )
                    .print(ostream);
        }

        mjolnir::Report report{
                kind, m_RedefineSpan.m_Source->m_Source,
                m_RedefineSpan.m_Span.start()
        };
        report.with_message(std::move(message))
                .with_label(
                        mjolnir::Label{m_RedefineSpan.m_Span}
                                .with_message("Redefinition here")
                                .with_color(mjolnir::colors::light_red)
                );

        if (defLabel.has_value()) {
            report.with_label(defLabel.value());
        }

        report.print(ostream);
    }
}// namespace jcc::diagnostics
