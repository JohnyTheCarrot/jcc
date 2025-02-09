#include "multiplicative_expression.h"

#include "binary_expression.h"
#include "preprocessor/preprocessor_iterator.h"
#include "primary_expression.h"

namespace jcc::parsing_sema {
    AstMultiplicativeExpression::AstMultiplicativeExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs,
            MultiplicativeOperator op, mjolnir::Span opSpan
    )
        : AstBinaryExpression{lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs), opSpan}
        , m_Operator{op} {
    }

    MultiplicativeOperator
    AstMultiplicativeExpression::GetOperator() const noexcept {
        return m_Operator;
    }

    void AstMultiplicativeExpression::Accept(ExpressionVisitor *visitor) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<MultiplicativeOperator>
    GetMultiplicativeOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::Asterisk:
                return MultiplicativeOperator::Multiplication;
            case tokenizer::Punctuator::Slash:
                return MultiplicativeOperator::Division;
            case tokenizer::Punctuator::Percent:
                return MultiplicativeOperator::Modulo;
            default:
                return std::nullopt;
        }
    }

    AstExpressionPtr ParseMultiplicativeExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<
                MultiplicativeOperator, AstMultiplicativeExpression>(
                GetMultiplicativeOperator, ParsePrimaryExpression, current, end
        );
    }
}// namespace jcc::parsing_sema
