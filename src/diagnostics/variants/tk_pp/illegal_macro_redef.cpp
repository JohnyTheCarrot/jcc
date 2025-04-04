#include "illegal_macro_redef.hpp"

#include <memory> // for sha...
#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...
#include "mjolnir/span.hpp"                                     // for Span

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
