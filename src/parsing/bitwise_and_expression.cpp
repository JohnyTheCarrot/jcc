#include "bitwise_and_expression.hpp"

#include "binary_expression.h"
#include "equality_expression.hpp"

namespace jcc::parsing {
    AstBitwiseAndExpression::AstBitwiseAndExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, BitwiseAndOperator,
            mjolnir::Span opSpan
    )
        : AstBinaryExpression{
                  lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs),
                  opSpan
          } {
    }

    void AstBitwiseAndExpression::AcceptOnExpression(ExpressionVisitor *visitor
    ) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<BitwiseAndOperator>
    GetBitwiseAndOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::Ampersand:
                return BitwiseAndOperator::And;
            default:
                return std::nullopt;
        }
    }

    AstExpressionPtr ParseBitwiseAndExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<
                BitwiseAndOperator, AstBitwiseAndExpression>(
                GetBitwiseAndOperator, ParseEqualityExpression, current, end
        );
    }
}// namespace jcc::parsing
