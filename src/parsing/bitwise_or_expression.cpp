#include "bitwise_or_expression.hpp"

#include "binary_expression.h"
#include "bitwise_xor_expression.hpp"

namespace jcc::parsing {
    AstBitwiseOrExpression::AstBitwiseOrExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, BitwiseOrOperator,
            mjolnir::Span opSpan
    )
        : AstBinaryExpression{
                  lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs),
                  opSpan
          } {
    }

    void AstBitwiseOrExpression::AcceptOnExpression(
            ExpressionVisitor *visitor
    ) const {
        return visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<BitwiseOrOperator>
    GetBitwiseOrOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::VerticalBar:
                return BitwiseOrOperator::Or;
            default:
                return std::nullopt;
        }
    }

    AstExpressionPtr ParseBitwiseOrExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<BitwiseOrOperator, AstBitwiseOrExpression>(
                GetBitwiseOrOperator, ParseBitwiseXorExpression, current, end
        );
    }
}// namespace jcc::parsing
