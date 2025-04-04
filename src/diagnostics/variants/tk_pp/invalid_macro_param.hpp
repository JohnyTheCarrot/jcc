#ifndef INVALID_MACRO_PARAM_HPP
#define INVALID_MACRO_PARAM_HPP

#include <memory>// for shared_ptr

#include "diagnostics/diagnostics.h"// for DiagnosticData
#include "mjolnir/span.hpp"         // for Span

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

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
