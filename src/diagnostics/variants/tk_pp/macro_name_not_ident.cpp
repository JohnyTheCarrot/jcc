#include "macro_name_not_ident.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    MacroNameNotIdent::MacroNameNotIdent(
            std::shared_ptr<Source> source, mjolnir::Span directiveSpan,
            mjolnir::Span macroNameSpan
    )
        : DiagnosticData{std::move(source), macroNameSpan.start(), mjolnir::BasicReportKind::Error}
        , m_DirectiveSpan{directiveSpan}
        , m_MacroNameSpan{macroNameSpan} {
    }

    void MacroNameNotIdent::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
