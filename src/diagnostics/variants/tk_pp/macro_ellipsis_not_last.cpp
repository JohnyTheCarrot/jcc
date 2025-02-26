#include "macro_ellipsis_not_last.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    MacroEllipsisNotLast::MacroEllipsisNotLast(
            std::shared_ptr<Source> source, mjolnir::Span macroSpan,
            mjolnir::Span commaSpan
    )
        : DiagnosticData{std::move(source), commaSpan.start(), mjolnir::BasicReportKind::Error}
        , m_MacroSpan{macroSpan}
        , m_CommaSpan{commaSpan} {
    }

    void MacroEllipsisNotLast::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
