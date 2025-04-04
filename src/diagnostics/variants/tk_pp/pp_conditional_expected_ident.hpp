#ifndef PP_CONDITIONAL_EXPECTED_IDENT_HPP
#define PP_CONDITIONAL_EXPECTED_IDENT_HPP

#include <memory>          // for shared_ptr
#include <mjolnir/span.hpp>// for Span

#include "diagnostics/diagnostics.h"// for DiagnosticData

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    struct PpConditionalExpectedIdent final : DiagnosticData {
        mjolnir::Span m_ConditionalSpan;
        mjolnir::Span m_UnexpectedTokenSpan;

        PpConditionalExpectedIdent(
                std::shared_ptr<Source> source, mjolnir::Span condSpan,
                mjolnir::Span unexpectedTokenSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//PP_CONDITIONAL_EXPECTED_IDENT_HPP
