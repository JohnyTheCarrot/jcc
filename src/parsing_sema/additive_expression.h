#ifndef ADDITIVE_EXPRESSION_H
#define ADDITIVE_EXPRESSION_H

#include "ast_node.h"
#include "parser.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing_sema {
    enum class AdditiveOperator { Addition, Subtraction };

    class AstAdditiveExpression final : public AstExpression {
        AstExpressionPtr m_Lhs;
        AstExpressionPtr m_Rhs;
        AdditiveOperator m_Operator;

    public:
        AstAdditiveExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs, AdditiveOperator op
        );

        [[nodiscard]]
        AstExpressionPtr::pointer GetLhs() const noexcept;

        [[nodiscard]]
        AstExpressionPtr::pointer GetRhs() const noexcept;

        [[nodiscard]]
        AdditiveOperator GetOperator() const noexcept;

        void Accept(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseAdditiveExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing_sema

#endif//ADDITIVE_EXPRESSION_H
