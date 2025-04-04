#include "shift_expression.h"

#include <functional>// for function
#include <memory>    // for unique_ptr
#include <optional>  // for optional, nullopt, nullopt_t
#include <utility>   // for move

#include "additive_expression.h"// for ParseAdditiveExpression
#include "binary_expression.h"  // for ParseBinaryExpression
#include "misc/Span.h"          // for Span
#include "mjolnir/span.hpp"     // for Span
#include "parsing/ast_node.h"   // for AstExpression, AstExpressionPtr
#include "tokenizer/token.h"    // for Punctuator

namespace jcc::parsing {
    AstShiftExpression::AstShiftExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs, ShiftOperator op,
            mjolnir::Span opSpan
    )
        : AstBinaryExpression{lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs), opSpan}
        , m_Operator{op} {
    }

    ShiftOperator AstShiftExpression::GetOperator() const noexcept {
        return m_Operator;
    }

    void AstShiftExpression::AcceptOnExpression(ExpressionVisitor *visitor
    ) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<ShiftOperator>
    GetShiftOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::LessThanLessThan:
                return ShiftOperator::Left;
            case tokenizer::Punctuator::GreaterThanGreaterThan:
                return ShiftOperator::Right;
            default:
                return std::nullopt;
        }
    }

    AstExpressionPtr ParseShiftExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<ShiftOperator, AstShiftExpression>(
                GetShiftOperator, ParseAdditiveExpression, current, end
        );
    }
}// namespace jcc::parsing
