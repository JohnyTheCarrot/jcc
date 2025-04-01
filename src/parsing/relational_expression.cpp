#include "relational_expression.hpp"

#include "binary_expression.h"
#include "shift_expression.h"
#include "tokenizer/token.h"

namespace jcc::parsing {
    AstRelationalExpression::AstRelationalExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, RelationalOperator op,
            mjolnir::Span opSpan
    )
        : AstBooleanBinaryExpression{lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs), opSpan}
        , m_RelationalOperator{op} {
    }

    RelationalOperator AstRelationalExpression::GetOperator() const noexcept {
        return m_RelationalOperator;
    }

    void AstRelationalExpression::AcceptOnExpression(
            ExpressionVisitor *visitor
    ) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<RelationalOperator>
    GetRelationalOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::LessThan:
                return RelationalOperator::LessThan;
            case tokenizer::Punctuator::GreaterThan:
                return RelationalOperator::GreaterThan;
            case tokenizer::Punctuator::LessThanEqual:
                return RelationalOperator::LessThanOrEqual;
            case tokenizer::Punctuator::GreaterThanEqual:
                return RelationalOperator::GreaterThanOrEqual;
            default:
                return std::nullopt;
        }
    }

    [[nodiscard]]

    AstExpressionPtr ParseRelationalExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<
                RelationalOperator, AstRelationalExpression>(
                GetRelationalOperator, ParseShiftExpression, current, end
        );
    }
}// namespace jcc::parsing
