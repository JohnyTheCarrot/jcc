#ifndef MACRO_NAME_NOT_IDENT_HPP
#define MACRO_NAME_NOT_IDENT_HPP
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
