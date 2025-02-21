#ifndef ILLEGAL_MACRO_REDEF_HPP
#define ILLEGAL_MACRO_REDEF_HPP

#include "diagnostics/diagnostics.h"
#include "misc/Span.h"

namespace jcc::diagnostics {
    struct IllegalMacroRedef final : DiagnosticData {
        Span m_DefSpan;
        Span m_RedefineSpan;

        IllegalMacroRedef(Span definedLoc, Span redefineLoc);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//ILLEGAL_MACRO_REDEF_HPP
