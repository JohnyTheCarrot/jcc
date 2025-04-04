#ifndef DIAGNOSTIC_DIRECTIVE_NO_TOKENS_HPP
#define DIAGNOSTIC_DIRECTIVE_NO_TOKENS_HPP

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
    enum class DiagnosticDirectiveKind {
        Warning,
        Error,
    };

    struct DiagnosticDirectiveNoTokens final : DiagnosticData {
        mjolnir::Span           m_Span;
        DiagnosticDirectiveKind m_Kind;

        DiagnosticDirectiveNoTokens(
                std::shared_ptr<Source> source, DiagnosticDirectiveKind kind,
                mjolnir::Span span
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//DIAGNOSTIC_DIRECTIVE_NO_TOKENS_HPP
