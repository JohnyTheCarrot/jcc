#include "shift_expression.h"

#include "additive_expression.h"
#include "binary_expression.h"
#include "misc/Diagnosis.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing_sema {
    AstShiftExpression::AstShiftExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, ShiftOperator op
    )
        : AstBinaryExpression{lhs->m_Span + rhs->m_Span, UsualArithmeticConversions(lhs->GetType(), rhs->GetType())}
        , m_Lhs{std::move(lhs)}
        , m_Rhs{std::move(rhs)}
        , m_Operator{op} {
    }

    AstExpressionPtr::pointer AstShiftExpression::GetLhs() const noexcept {
        return m_Lhs.get();
    }

    AstExpressionPtr::pointer AstShiftExpression::GetRhs() const noexcept {
        return m_Rhs.get();
    }

    ShiftOperator AstShiftExpression::GetOperator() const noexcept {
        return m_Operator;
    }

    void AstShiftExpression::Accept(ExpressionVisitor *visitor) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<ShiftOperator>
    GetShiftOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::LessThanLessThan:
                return ShiftOperator::Left;
            case tokenizer::Punctuator::GreaterThanGreaterThan:
                return ShiftOperator::Right;
            default:
                return std::nullopt;
        }
    }

    AstExpressionPtr ParseShiftExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<ShiftOperator, AstShiftExpression>(
                GetShiftOperator, ParseAdditiveExpression, current, end
        );
    }
}// namespace jcc::parsing_sema
