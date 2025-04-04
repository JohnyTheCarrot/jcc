#ifndef ILLEGAL_MACRO_REDEF_HPP
#define ILLEGAL_MACRO_REDEF_HPP

#include "diagnostics/diagnostics.h"// for DiagnosticData
#include "misc/Span.h"              // for Span

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    struct IllegalMacroRedef final : DiagnosticData {
        Span m_DefSpan;
        Span m_RedefineSpan;

        IllegalMacroRedef(Span definedLoc, Span redefineLoc);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//ILLEGAL_MACRO_REDEF_HPP
