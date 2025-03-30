#ifndef MULTIPLICATIVE_EXPRESSION_H
#define MULTIPLICATIVE_EXPRESSION_H

#include "ast_node.h"
#include "parser.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    enum class MultiplicativeOperator { Multiplication, Division, Modulo };

    class AstMultiplicativeExpression final : public AstBinaryExpression {
        MultiplicativeOperator m_Operator;

    public:
        AstMultiplicativeExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs,
                MultiplicativeOperator op, mjolnir::Span opSpan
        );

        [[nodiscard]]
        MultiplicativeOperator GetOperator() const noexcept;

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseMultiplicativeExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//MULTIPLICATIVE_EXPRESSION_H
