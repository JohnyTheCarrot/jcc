#ifndef SEMA_VISITOR_H
#define SEMA_VISITOR_H

#include "parsing/ast_node.h"

namespace jcc::visitors {
    class SemaVisitor final : public parsing::AstNodeVisitor {
        void SemaBitwiseExpression(
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
        Visit(parsing::SpecifierQualifierList const *specifierQualifierList
        ) override;

        void Visit(parsing::AstTypeName const *astTypeName) override;

        void
        Visit(parsing::AstRelationalExpression const *astRelationalExpr
        ) override;

        void
        Visit(parsing::AstEqualityExpression const *astEqualityExpression
        ) override;

        void
        Visit(parsing::AstBitwiseAndExpression const *astBitwiseAndExpr
        ) override;

        void
        Visit(parsing::AstBitwiseXorExpression const *astBitwiseXorExpr
        ) override;
    };
}// namespace jcc::visitors

#endif//SEMA_VISITOR_H
