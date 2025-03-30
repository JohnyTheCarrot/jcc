#include "additive_expression.h"

#include "binary_expression.h"
#include "multiplicative_expression.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing {
    AstAdditiveExpression::AstAdditiveExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, AdditiveOperator op,
            mjolnir::Span opSpan
    )
        : AstBinaryExpression{lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs), opSpan}
        , m_Operator{op} {
    }

    AdditiveOperator AstAdditiveExpression::GetOperator() const noexcept {
        return m_Operator;
    }

    void AstAdditiveExpression::AcceptOnExpression(
            ExpressionVisitor *visitor
    ) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<AdditiveOperator>
    GetAdditiveOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::Plus:
                return AdditiveOperator::Addition;
            case tokenizer::Punctuator::Minus:
                return AdditiveOperator::Subtraction;
            default:
                return std::nullopt;
        }
    }

    AstExpressionPtr ParseAdditiveExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<AdditiveOperator, AstAdditiveExpression>(
                GetAdditiveOperator, ParseMultiplicativeExpression, current, end
        );
    }
}// namespace jcc::parsing
