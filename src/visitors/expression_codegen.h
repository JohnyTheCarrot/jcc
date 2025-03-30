#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <llvm/IR/Value.h>

#include "parsing/ast_node.h"

namespace jcc::visitors {
    class ExpressionCodegenVisitor final
        : public parsing::ExpressionVisitor {
        llvm::Value *m_Value{};

        std::pair<llvm::Value *, llvm::Value *>
        PrepareOperands(parsing::AstBinaryExpression const *astExpr);

    public:
        void
        Visit(parsing::AstIntegerConstant const *astIntConst) override;

        void
        Visit(parsing::AstMultiplicativeExpression const
                      *astMultiplicativeExpr) override;

        void
        Visit(parsing::AstAdditiveExpression const *astAdditiveExpr
        ) override;

        void
        Visit(parsing::AstShiftExpression const *astShiftExpr) override;

        void
        Visit(parsing::AstFloatingConstant const *astFloatingConst
        ) override;

        void Visit(parsing::AstCastExpression const *astCastExpr) override;

        [[nodiscard]]
        llvm::Value *GetValue() noexcept;
    };
}// namespace jcc::visitors

#endif//EXPRESSIONS_H
