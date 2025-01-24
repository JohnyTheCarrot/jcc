#include "multiplicative_expression.h"

#include "preprocessor/preprocessor_iterator.h"
#include "primary_expression.h"

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

    AstExpressionPtr ParseMultiplicativeExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        // TODO: supposed to be a cast expression
        auto lhsExpr{ParsePrimaryExpression(current, end)};

        if (lhsExpr == nullptr)
            return nullptr;

        ++current;

        while (current != end) {
            if (tokenizer::Token nextToken{*current};
                nextToken.Is<tokenizer::Punctuator>()) {

                MultiplicativeOperator op;

                switch (std::get<tokenizer::Punctuator>(nextToken.m_Value)) {
                    case tokenizer::Punctuator::Asterisk:
                        op = MultiplicativeOperator::Multiplication;
                        break;
                    case tokenizer::Punctuator::Slash:
                        op = MultiplicativeOperator::Division;
                        break;
                    case tokenizer::Punctuator::Percent:
                        op = MultiplicativeOperator::Modulo;
                        break;
                    default:
                        return lhsExpr;
                }

                ++current;
                if (current == end) {
                    throw std::runtime_error{"Expected expression"};
                }

                // TODO: supposed to be a cast expression
                auto rhsExpr{ParsePrimaryExpression(current, end)};
                if (rhsExpr == nullptr) {
                    // TODO: Use FatalCompilerError with span info
                    throw std::runtime_error{"Expected expression"};
                }

                lhsExpr = std::make_unique<AstMultiplicativeExpression>(
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
