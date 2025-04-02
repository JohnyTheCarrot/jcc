#ifndef BITWISE_AND_EXPRESSION_HPP
#define BITWISE_AND_EXPRESSION_HPP

#include "ast_node.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    // This enum class only exists so we satisfy the ParseBinaryExpression requirements.
    enum class BitwiseAndOperator { And };

    class AstBitwiseAndExpression final : public AstBinaryExpression {
    public:
        // This constructor only takes the operator for the sake of satisfying ParseBinaryExpression.
        AstBitwiseAndExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs, BitwiseAndOperator,
                mjolnir::Span opSpan
        );

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseBitwiseAndExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//BITWISE_AND_EXPRESSION_HPP
