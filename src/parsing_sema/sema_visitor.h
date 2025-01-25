#ifndef SEMA_VISITOR_H
#define SEMA_VISITOR_H

#include "ast_node.h"

namespace jcc::parsing_sema {
    class SemaVisitor final : public AstNodeVisitor {
    public:
        void Visit(AstIntegerConstant const *astIntConst) override;

        void
        Visit(AstMultiplicativeExpression const *astMultiplicativeExpr
        ) override;

        void Visit(AstAdditiveExpression const *astAdditiveExpr) override;

        void Visit(AstShiftExpression const *astShiftExpr) override;
    };
}// namespace jcc::parsing_sema

#endif//SEMA_VISITOR_H
