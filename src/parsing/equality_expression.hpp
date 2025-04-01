#ifndef EQUALITY_EXPRESSION_HPP
#define EQUALITY_EXPRESSION_HPP
#include "ast_node.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    enum class EqualityOperator { Equal, NotEqual };

    class AstEqualityExpression final : public AstBooleanBinaryExpression {
        EqualityOperator m_EqualityOperator;

    public:
        AstEqualityExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs,
                EqualityOperator equalityOperator, mjolnir::Span opSpan
        );

        [[nodiscard]]
        EqualityOperator GetOperator() const noexcept;

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseEqualityExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//EQUALITY_EXPRESSION_HPP
