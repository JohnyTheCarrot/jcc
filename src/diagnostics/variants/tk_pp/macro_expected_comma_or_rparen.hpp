#ifndef MACRO_EXPECTED_COMMA_OR_RPAREN_HPP
#define MACRO_EXPECTED_COMMA_OR_RPAREN_HPP
#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct MacroExpectedCommaOrRparen final : DiagnosticData {
        mjolnir::Span m_MacroSpan;
        mjolnir::Span m_Span;

        MacroExpectedCommaOrRparen(
                std::shared_ptr<Source> source, mjolnir::Span macroSpan,
                mjolnir::Span span
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//MACRO_EXPECTED_COMMA_OR_RPAREN_HPP
