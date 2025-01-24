#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <llvm/IR/Value.h>

#include "parsing_sema/ast_node.h"

namespace jcc::codegen {
    class ExpressionCodegenVisitor final
        : public parsing_sema::ExpressionVisitor {
        mutable llvm::Value *m_Value{};

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

        [[nodiscard]]
        llvm::Value *GetValue() const noexcept;
    };
}// namespace jcc::codegen

#endif//EXPRESSIONS_H
