#ifndef MACRO_NAME_NOT_IDENT_HPP
#define MACRO_NAME_NOT_IDENT_HPP
#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct MacroNameNotIdent final : DiagnosticData {
        mjolnir::Span m_DirectiveSpan;
        mjolnir::Span m_MacroNameSpan;

        MacroNameNotIdent(
                std::shared_ptr<Source> source, mjolnir::Span directiveSpan,
                mjolnir::Span macroNameSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//MACRO_NAME_NOT_IDENT_HPP
