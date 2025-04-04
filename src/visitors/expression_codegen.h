#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <llvm/IR/Value.h>// for Value
#include <utility>        // for pair

#include "parsing/ast_node.h"// for AstBinaryExpression (ptr only), Expres...

namespace jcc {
    namespace parsing {
        class AstAdditiveExpression;
        class AstBitwiseAndExpression;
        class AstBitwiseOrExpression;
        class AstBitwiseXorExpression;
        class AstCastExpression;
        class AstEqualityExpression;
        class AstFloatingConstant;
        class AstIntegerConstant;
        class AstLogicalAndExpression;
        class AstMultiplicativeExpression;
        class AstRelationalExpression;
        class AstShiftExpression;

        namespace types {
            class ValueType;
        }// namespace types
    }// namespace parsing
}// namespace jcc

namespace jcc::visitors {
    class ExpressionCodegenVisitor final : public parsing::ExpressionVisitor {
        llvm::Value *m_Value{};

        std::pair<llvm::Value *, llvm::Value *> PrepareOperands(
                parsing::AstBinaryExpression const *astExpr,
                parsing::types::ValueType           type
        );

    public:
        void Visit(parsing::AstIntegerConstant const *astIntConst) override;

        void
        Visit(parsing::AstMultiplicativeExpression const *astMultiplicativeExpr
        ) override;

        void Visit(parsing::AstAdditiveExpression const *astAdditiveExpr
        ) override;

        void Visit(parsing::AstShiftExpression const *astShiftExpr) override;

        void Visit(parsing::AstFloatingConstant const *astFloatingConst
        ) override;

        void Visit(parsing::AstCastExpression const *astCastExpr) override;

        void Visit(parsing::AstRelationalExpression const *astRelationalExpr
        ) override;

        void Visit(parsing::AstEqualityExpression const *astEqualityExpression
        ) override;

        void Visit(parsing::AstBitwiseAndExpression const *astBitwiseAndExpr
        ) override;

        void Visit(parsing::AstBitwiseXorExpression const *astBitwiseXorExpr
        ) override;

        void Visit(parsing::AstBitwiseOrExpression const *astBitwiseOrExpr
        ) override;

        void Visit(parsing::AstLogicalAndExpression const *astLogicalAndExpr
        ) override;

        [[nodiscard]]
        llvm::Value *GetValue() noexcept;
    };
}// namespace jcc::visitors

#endif//EXPRESSIONS_H
