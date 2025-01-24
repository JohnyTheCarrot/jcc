#ifndef MULTIPLICATIVE_EXPRESSION_H
#define MULTIPLICATIVE_EXPRESSION_H
#include "ast_node.h"
#include "parser.h"
#include "primary_expression.h"

namespace jcc::parsing_sema {
    enum class MultiplicativeOperator { Multiplication, Division, Modulo };

    class AstMultiplicativeExpression final : public AstExpression {
        AstExpressionPtr       m_Lhs;
        AstExpressionPtr       m_Rhs;
        MultiplicativeOperator m_Operator;

    public:
        AstMultiplicativeExpression(
                AstExpressionPtr lhs, AstExpressionPtr rhs,
                MultiplicativeOperator op
        );

        [[nodiscard]]
        AstExpressionPtr::pointer GetLhs() const noexcept;

        [[nodiscard]]
        AstExpressionPtr::pointer GetRhs() const noexcept;

        [[nodiscard]]
        MultiplicativeOperator GetOperator() const noexcept;

        void Accept(ExpressionVisitor *visitor) const override;
    };

    template<typename It>
        requires TokenIterator<It>
    [[nodiscard]]
    AstExpressionPtr ParseMultiplicativeExpression(It current, It end) {
        if (current == end)
            return nullptr;

        // TODO: supposed to be a cast expression
        auto lhsExpr{ParsePrimaryExpression(*current)};

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

                auto rhsExpr{ParsePrimaryExpression(*current)};
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

#endif//MULTIPLICATIVE_EXPRESSION_H
