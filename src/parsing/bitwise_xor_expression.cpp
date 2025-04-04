#include "bitwise_xor_expression.hpp"

#include "binary_expression.h"
#include "bitwise_and_expression.hpp"
#include "tokenizer/token.h"

namespace jcc::parsing {
    AstBitwiseXorExpression::AstBitwiseXorExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, BitwiseXorOperator,
            mjolnir::Span opSpan
    )
        : AstBinaryExpression{
                  lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs),
                  opSpan
          } {
    }

    void AstBitwiseXorExpression::AcceptOnExpression(ExpressionVisitor *visitor
    ) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<BitwiseXorOperator>
    GetBitwiseXorOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::Caret:
                return BitwiseXorOperator::Xor;
            default:
                return std::nullopt;
        }
    }

    AstExpressionPtr ParseBitwiseXorExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<
                BitwiseXorOperator, AstBitwiseXorExpression>(
                GetBitwiseXorOperator, ParseBitwiseAndExpression, current, end
        );
    }
}// namespace jcc::parsing
