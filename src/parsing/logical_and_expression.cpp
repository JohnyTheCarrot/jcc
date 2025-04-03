#include "logical_and_expression.hpp"

#include "binary_expression.h"
#include "bitwise_or_expression.hpp"
#include "tokenizer/token.h"

namespace jcc::parsing {
    AstLogicalAndExpression::AstLogicalAndExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, LogicalAndOperator,
            mjolnir::Span opSpan
    )
        : AstBooleanBinaryExpression{
                  lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs),
                  opSpan
          } {
    }

    void AstLogicalAndExpression::AcceptOnExpression(
            ExpressionVisitor *visitor
    ) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<LogicalAndOperator>
    GetLogicalAndOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::AmpersandAmpersand:
                return LogicalAndOperator::And;
            default:
                return std::nullopt;
        }
    }

    AstExpressionPtr ParseLogicalAndExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<
                LogicalAndOperator, AstLogicalAndExpression>(
                GetLogicalAndOperator, ParseBitwiseOrExpression, current, end
        );
    }
}// namespace jcc::parsing
