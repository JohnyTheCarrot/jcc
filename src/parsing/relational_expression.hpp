#ifndef RELATIONAL_EXPRESSION_HPP
#define RELATIONAL_EXPRESSION_HPP
#include "ast_node.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    enum class RelationalOperator {
        LessThan,
        GreaterThan,
        LessThanOrEqual,
        GreaterThanOrEqual
    };

    class AstRelationalExpression final : public AstBooleanBinaryExpression {
        RelationalOperator m_RelationalOperator;

    public:
        AstRelationalExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs,
                RelationalOperator op, mjolnir::Span opSpan
        );

        [[nodiscard]]
        RelationalOperator GetOperator() const noexcept;

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseRelationalExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//RELATIONAL_EXPRESSION_HPP
