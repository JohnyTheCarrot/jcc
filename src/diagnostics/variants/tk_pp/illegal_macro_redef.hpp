#ifndef ILLEGAL_MACRO_REDEF_HPP
#define ILLEGAL_MACRO_REDEF_HPP

#include "diagnostics/diagnostics.h"
#include "misc/Span.h"

namespace jcc::diagnostics {
    class IllegalMacroRedef final : public DiagnosticData {
        Span m_DefSpan;
        Span m_RedefineSpan;

    public:
        IllegalMacroRedef(Span definedLoc, Span redefineLoc);

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//ILLEGAL_MACRO_REDEF_HPP
