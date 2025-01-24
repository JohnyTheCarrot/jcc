#include "shift_expression.h"

#include "additive_expression.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing_sema {
    AstShiftExpression::AstShiftExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, ShiftOperator op
    )
        : AstExpression{UsualArithmeticConversions(
                  lhs->GetType(), rhs->GetType()
          )}
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

    AstExpressionPtr ParseShiftExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        auto lhsExpr{ParseAdditiveExpression(current, end)};

        if (lhsExpr == nullptr)
            return nullptr;

        while (current != end) {
            if (tokenizer::Token nextToken{*current};
                nextToken.Is<tokenizer::Punctuator>()) {

                ShiftOperator op;

                switch (std::get<tokenizer::Punctuator>(nextToken.m_Value)) {
                    case tokenizer::Punctuator::LessThanLessThan:
                        op = ShiftOperator::Left;
                        break;
                    case tokenizer::Punctuator::GreaterThanGreaterThan:
                        op = ShiftOperator::Right;
                        break;
                    default:
                        return lhsExpr;
                }

                ++current;
                if (current == end) {
                    throw std::runtime_error{"Expected expression"};
                }

                auto rhsExpr{ParseAdditiveExpression(current, end)};
                if (rhsExpr == nullptr) {
                    // TODO: Use FatalCompilerError with span info
                    throw std::runtime_error{"Expected expression"};
                }

                lhsExpr = std::make_unique<AstShiftExpression>(
                        std::move(lhsExpr), std::move(rhsExpr), op
                );
                ++current;

                continue;
            }

            return lhsExpr;
        }

        return lhsExpr;
    }
}// namespace jcc::parsing_sema
