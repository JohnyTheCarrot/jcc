#ifndef MACRO_EXPECTED_COMMA_OR_RPAREN_HPP
#define MACRO_EXPECTED_COMMA_OR_RPAREN_HPP
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
