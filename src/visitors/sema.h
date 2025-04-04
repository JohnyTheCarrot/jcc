#ifndef SEMA_VISITOR_H
#define SEMA_VISITOR_H

#include "parsing/ast_node.h"// for AstBinaryExpression (ptr only), AstNod...

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
        class AstTypeName;
        class SpecifierQualifierList;
    }// namespace parsing
}// namespace jcc

namespace jcc::visitors {
    class SemaVisitor final : public parsing::AstNodeVisitor {
        void
        SemaBitwiseExpression(parsing::AstBinaryExpression const *astBinaryExpr
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

        void Visit(parsing::SpecifierQualifierList const *specifierQualifierList
        ) override;

        void Visit(parsing::AstTypeName const *astTypeName) override;

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
    };
}// namespace jcc::visitors

#endif//SEMA_VISITOR_H
