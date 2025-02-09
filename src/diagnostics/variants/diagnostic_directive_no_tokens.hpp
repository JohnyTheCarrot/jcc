#ifndef DIAGNOSTIC_DIRECTIVE_NO_TOKENS_HPP
#define DIAGNOSTIC_DIRECTIVE_NO_TOKENS_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    enum class DiagnosticDirectiveKind {
        Warning,
        Error,
    };

    class DiagnosticDirectiveNoTokens final : public DiagnosticData {
        mjolnir::Span           m_Span;
        DiagnosticDirectiveKind m_Kind;

    public:
        DiagnosticDirectiveNoTokens(
                std::shared_ptr<Source> source, DiagnosticDirectiveKind kind,
                mjolnir::Span span
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//DIAGNOSTIC_DIRECTIVE_NO_TOKENS_HPP
