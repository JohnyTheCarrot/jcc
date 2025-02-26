#ifndef EXPECTED_EXPRESSION_HPP
#define EXPECTED_EXPRESSION_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct ExpectedExpression final : DiagnosticData {
        mjolnir::Span                m_LhsSpan;
        mjolnir::Span                m_OpSpan;
        std::optional<mjolnir::Span> m_RhsSpan;

        /**
         * @param source  The source of the diagnostic
         * @param lhsSpan The span of the lhs
         * @param opSpan  The span of the operator
         * @param rhsSpan The span of the right-hand side of the expression, std::nullopt if no RHS is presend (EOF)
         */
        ExpectedExpression(
                std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
                mjolnir::Span                       opSpan,
                std::optional<mjolnir::Span> const &rhsSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//EXPECTED_EXPRESSION_HPP
