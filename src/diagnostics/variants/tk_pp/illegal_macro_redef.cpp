#include "illegal_macro_redef.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    IllegalMacroRedef::IllegalMacroRedef(Span definedLoc, Span redefineLoc)
        : DiagnosticData{redefineLoc.m_Source, redefineLoc.m_Span.start(), mjolnir::BasicReportKind::Error}
        , m_DefSpan{std::move(definedLoc)}
        , m_RedefineSpan{std::move(redefineLoc)} {
    }

    void IllegalMacroRedef::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
