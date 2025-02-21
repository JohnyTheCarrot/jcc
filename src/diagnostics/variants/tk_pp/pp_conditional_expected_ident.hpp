#ifndef PP_CONDITIONAL_EXPECTED_IDENT_HPP
#define PP_CONDITIONAL_EXPECTED_IDENT_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

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
