#ifndef MACRO_INVOC_EXPECTED_LPAREN_HPP
#define MACRO_INVOC_EXPECTED_LPAREN_HPP

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
    struct MacroInvocExpectedLparen final : DiagnosticData {
        mjolnir::Span m_MacroNameSpan;
        mjolnir::Span m_NextTokenSpan;

        MacroInvocExpectedLparen(
                std::shared_ptr<Source> source, mjolnir::Span macroNameSpan,
                mjolnir::Span nextTokenSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//MACRO_INVOC_EXPECTED_LPAREN_HPP
