#ifndef SHIFT_EXPRESSION_H
#define SHIFT_EXPRESSION_H

#include "ast_node.h"// for AstExpressionPtr, AstBinaryExpression

namespace mjolnir {
    class Span;
}// namespace mjolnir

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    enum class ShiftOperator { Left, Right };

    class AstShiftExpression final : public AstBinaryExpression {
        ShiftOperator m_Operator;

    public:
        AstShiftExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs, ShiftOperator op,
                mjolnir::Span opSpan
        );

        [[nodiscard]]
        ShiftOperator GetOperator() const noexcept;

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseShiftExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//SHIFT_EXPRESSION_H
