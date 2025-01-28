#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <llvm/IR/Value.h>

#include "parsing_sema/ast_node.h"

namespace jcc::codegen {
    class ExpressionCodegenVisitor final
        : public parsing_sema::ExpressionVisitor {
        llvm::Value *m_Value{};

        std::pair<llvm::Value *, llvm::Value *>
        PrepareOperands(parsing_sema::AstBinaryExpression const *astExpr);

    public:
        void
        Visit(parsing_sema::AstIntegerConstant const *astIntConst) override;

        void
        Visit(parsing_sema::AstMultiplicativeExpression const
                      *astMultiplicativeExpr) override;

        void
        Visit(parsing_sema::AstAdditiveExpression const *astAdditiveExpr
        ) override;

        void
        Visit(parsing_sema::AstShiftExpression const *astShiftExpr) override;

        void
        Visit(parsing_sema::AstFloatingConstant const *astFloatingConst
        ) override;

        [[nodiscard]]
        llvm::Value *GetValue() noexcept;
    };
}// namespace jcc::codegen

#endif//EXPRESSIONS_H
