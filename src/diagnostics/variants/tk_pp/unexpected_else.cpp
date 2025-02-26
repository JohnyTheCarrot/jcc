#include "unexpected_else.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    UnexpectedElse::UnexpectedElse(
            std::shared_ptr<Source> source, mjolnir::Span elseSpan,
            mjolnir::Span unexpectedElseSpan
    )
        : DiagnosticData{std::move(source), unexpectedElseSpan.start(), mjolnir::BasicReportKind::Error}
        , m_ElseSpan{elseSpan}
        , m_UnexpectedElseSpan{unexpectedElseSpan} {
    }

    void UnexpectedElse::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
