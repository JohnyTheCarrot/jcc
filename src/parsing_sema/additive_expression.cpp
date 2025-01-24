#include "additive_expression.h"

#include "multiplicative_expression.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing_sema {
    AstAdditiveExpression::AstAdditiveExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, AdditiveOperator op
    )
        : AstExpression{UsualArithmeticConversions(
                  lhs->GetType(), rhs->GetType()
          )}
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

    AstExpressionPtr ParseAdditiveExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        auto lhsExpr{ParseMultiplicativeExpression(current, end)};

        if (lhsExpr == nullptr)
            return nullptr;

        while (current != end) {
            if (tokenizer::Token nextToken{*current};
                nextToken.Is<tokenizer::Punctuator>()) {

                AdditiveOperator op;

                switch (std::get<tokenizer::Punctuator>(nextToken.m_Value)) {
                    case tokenizer::Punctuator::Plus:
                        op = AdditiveOperator::Addition;
                        break;
                    case tokenizer::Punctuator::Minus:
                        op = AdditiveOperator::Subtraction;
                        break;
                    default:
                        return lhsExpr;
                }

                ++current;
                if (current == end) {
                    throw std::runtime_error{"Expected expression"};
                }

                auto rhsExpr{ParseMultiplicativeExpression(current, end)};
                if (rhsExpr == nullptr) {
                    // TODO: Use FatalCompilerError with span info
                    throw std::runtime_error{"Expected expression"};
                }

                lhsExpr = std::make_unique<AstAdditiveExpression>(
                        std::move(lhsExpr), std::move(rhsExpr), op
                );

                continue;
            }

            return lhsExpr;
        }

        return lhsExpr;
    }
}// namespace jcc::parsing_sema
