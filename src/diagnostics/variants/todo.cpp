#include "todo.hpp"

namespace jcc::diagnostics {
    TodoError::TodoError(std::shared_ptr<Source> source, mjolnir::Span span)
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void TodoError::Print(std::ostream &ostream) const {
        StartReport()
                .with_message("Compiler feature unimplemented.")
                .with_label(mjolnir::Label{m_Span})
                .print(ostream);
    }
}// namespace jcc::diagnostics
