#ifndef ADDITIVE_EXPRESSION_H
#define ADDITIVE_EXPRESSION_H

#include "ast_node.h"
#include "parser.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing_sema {
    enum class AdditiveOperator { Addition, Subtraction };

    class AstAdditiveExpression final : public AstBinaryExpression {
        AdditiveOperator m_Operator;

    public:
        AstAdditiveExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs, AdditiveOperator op,
                mjolnir::Span opSpan
        );

        [[nodiscard]]
        AdditiveOperator GetOperator() const noexcept;

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseAdditiveExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing_sema

#endif//ADDITIVE_EXPRESSION_H
