#include "additive_expression.h"

#include "binary_expression.h"
#include "multiplicative_expression.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing_sema {
    AstAdditiveExpression::AstAdditiveExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, AdditiveOperator op
    )
        : AstBinaryExpression{lhs->m_Span + rhs->m_Span, UsualArithmeticConversions(lhs->GetType(), rhs->GetType())}
        , m_Lhs{std::move(lhs)}
        , m_Rhs{std::move(rhs)}
        , m_Operator{op} {
    }

    AstExpressionPtr::pointer AstAdditiveExpression::GetLhs() const noexcept {
        return m_Lhs.get();
    }

    AstExpressionPtr::pointer AstAdditiveExpression::GetRhs() const noexcept {
        return m_Rhs.get();
    }

    AdditiveOperator AstAdditiveExpression::GetOperator() const noexcept {
        return m_Operator;
    }

    void AstAdditiveExpression::Accept(ExpressionVisitor *visitor) const {
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
}// namespace jcc::parsing_sema
