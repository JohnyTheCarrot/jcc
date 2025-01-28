#include "multiplicative_expression.h"

#include "binary_expression.h"
#include "preprocessor/preprocessor_iterator.h"
#include "primary_expression.h"

namespace jcc::parsing_sema {
    AstMultiplicativeExpression::AstMultiplicativeExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs,
            MultiplicativeOperator op
    )
        : AstBinaryExpression{lhs->m_Span + rhs->m_Span, UsualArithmeticConversions(lhs->GetType(), rhs->GetType())}
        , m_Lhs{std::move(lhs)}
        , m_Rhs{std::move(rhs)}
        , m_Operator{op} {
    }

    AstExpressionPtr::pointer
    AstMultiplicativeExpression::GetLhs() const noexcept {
        return m_Lhs.get();
    }

    AstExpressionPtr::pointer
    AstMultiplicativeExpression::GetRhs() const noexcept {
        return m_Rhs.get();
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
