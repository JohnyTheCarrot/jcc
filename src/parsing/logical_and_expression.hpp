#ifndef LOGICAL_AND_EXPRESSION_HPP
#define LOGICAL_AND_EXPRESSION_HPP

#include "ast_node.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    // This enum class only exists so we satisfy the ParseBinaryExpression requirements.
    enum class LogicalAndOperator { And };

    // This constructor only takes the operator for the sake of satisfying ParseBinaryExpression.
    class AstLogicalAndExpression final : public AstBooleanBinaryExpression {
    public:
        AstLogicalAndExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs, LogicalAndOperator,
                mjolnir::Span opSpan
        );

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseLogicalAndExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//LOGICAL_AND_EXPRESSION_HPP
