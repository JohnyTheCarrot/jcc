#include "equality_expression.hpp"

#include "binary_expression.h"
#include "relational_expression.hpp"
#include "tokenizer/token.h"

namespace jcc::parsing {
    AstEqualityExpression::AstEqualityExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs,
            EqualityOperator equalityOperator, mjolnir::Span opSpan
    )
        : AstBooleanBinaryExpression{lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs), opSpan}
        , m_EqualityOperator{equalityOperator} {
    }

    EqualityOperator AstEqualityExpression::GetOperator() const noexcept {
        return m_EqualityOperator;
    }

    void AstEqualityExpression::AcceptOnExpression(ExpressionVisitor *visitor
    ) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<EqualityOperator>
    GetEqualityOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::EqualEqual:
                return EqualityOperator::Equal;
            case tokenizer::Punctuator::ExclamationMarkEqual:
                return EqualityOperator::NotEqual;
            default:
                return std::nullopt;
        }
    }

    [[nodiscard]]

    AstExpressionPtr ParseEqualityExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<EqualityOperator, AstEqualityExpression>(
                GetEqualityOperator, ParseRelationalExpression, current, end
        );
    }
}// namespace jcc::parsing
