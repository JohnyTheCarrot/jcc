#ifndef INVALID_MACRO_PARAM_HPP
#define INVALID_MACRO_PARAM_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct InvalidMacroParam final : DiagnosticData {
        mjolnir::Span m_MacroDefSpan;
        mjolnir::Span m_InvalidParamSpan;

        InvalidMacroParam(
                std::shared_ptr<Source> source, mjolnir::Span macroDefSpan,
                mjolnir::Span invalidParamSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//INVALID_MACRO_PARAM_HPP
