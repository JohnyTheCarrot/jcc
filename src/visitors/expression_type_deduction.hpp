#ifndef EXPRESSION_TYPE_DEDUCTION_HPP
#define EXPRESSION_TYPE_DEDUCTION_HPP

#include "parsing/ast_node.h"

namespace jcc::visitors {
    class ExpressionTypeDeductionVisitor final
        : public parsing::ExpressionVisitor {
        void BinaryExprTypeDeduction(
                parsing::AstBinaryExpression const *astBinaryExpr
        );

    public:
        void Visit(parsing::AstIntegerConstant const *astIntConst) override;

        void
        Visit(parsing::AstMultiplicativeExpression const *astMultiplicativeExpr
        ) override;

        void
        Visit(parsing::AstAdditiveExpression const *astAdditiveExpr) override;

        void Visit(parsing::AstShiftExpression const *astShiftExpr) override;

        void
        Visit(parsing::AstFloatingConstant const *astFloatingConst) override;

        void Visit(parsing::AstCastExpression const *astCastExpr) override;

        void
        Visit(parsing::AstRelationalExpression const *astRelationalExpr
        ) override;
    };
}// namespace jcc::visitors

#endif//EXPRESSION_TYPE_DEDUCTION_HPP
