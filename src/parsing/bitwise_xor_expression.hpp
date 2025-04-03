#ifndef BITWISE_XOR_EXPRESSION_HPP
#define BITWISE_XOR_EXPRESSION_HPP

#include "ast_node.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    enum class BitwiseXorOperator { Xor };

    class AstBitwiseXorExpression final : public AstBinaryExpression {
    public:
        // This constructor only takes the operator for the sake of satisfying ParseBinaryExpression.
        AstBitwiseXorExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs, BitwiseXorOperator,
                mjolnir::Span opSpan
        );

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseBitwiseXorExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//BITWISE_XOR_EXPRESSION_HPP
