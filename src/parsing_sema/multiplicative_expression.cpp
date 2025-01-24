#include "multiplicative_expression.h"

namespace jcc::parsing_sema {
    AstMultiplicativeExpression::AstMultiplicativeExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs,
            MultiplicativeOperator op
    )
        : AstExpression{UsualArithmeticConversions(
                  lhs->GetType(), rhs->GetType()
          )}
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
}// namespace jcc::parsing_sema
