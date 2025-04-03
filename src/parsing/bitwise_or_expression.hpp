#ifndef BITWISE_OR_EXPRESSION_HPP
#define BITWISE_OR_EXPRESSION_HPP

#include "ast_node.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    // This enum class only exists so we satisfy the ParseBinaryExpression requirements.
    enum class BitwiseOrOperator { Or };

    class AstBitwiseOrExpression final : public AstBinaryExpression {
    public:
        // This constructor only takes the operator for the sake of satisfying ParseBinaryExpression.
        AstBitwiseOrExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs, BitwiseOrOperator,
                mjolnir::Span opSpan
        );

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseBitwiseOrExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//BITWISE_OR_EXPRESSION_HPP
