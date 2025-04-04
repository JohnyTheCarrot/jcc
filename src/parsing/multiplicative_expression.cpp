#include "multiplicative_expression.h"

#include <functional>// for function
#include <memory>    // for unique_ptr
#include <optional>  // for optional, nullopt, nullopt_t
#include <utility>   // for move

#include "binary_expression.h"// for ParseBinaryExpression
#include "cast_expression.hpp"// for ParseCastExpression
#include "misc/Span.h"        // for Span
#include "mjolnir/span.hpp"   // for Span
#include "parsing/ast_node.h" // for AstExpression, AstExpressionPtr, AstB...
#include "tokenizer/token.h"  // for Punctuator

namespace jcc::parsing {
    AstMultiplicativeExpression::AstMultiplicativeExpression(
            AstExpressionPtr lhs, AstExpressionPtr rhs,
            MultiplicativeOperator op, mjolnir::Span opSpan
    )
        : AstBinaryExpression{lhs->m_Span + rhs->m_Span, std::move(lhs), std::move(rhs), opSpan}
        , m_Operator{op} {
    }

    MultiplicativeOperator
    AstMultiplicativeExpression::GetOperator() const noexcept {
        return m_Operator;
    }

    void
    AstMultiplicativeExpression::AcceptOnExpression(ExpressionVisitor *visitor
    ) const {
        visitor->Visit(this);
    }

    [[nodiscard]]
    std::optional<MultiplicativeOperator>
    GetMultiplicativeOperator(tokenizer::Punctuator punctuator) {
        switch (punctuator) {
            case tokenizer::Punctuator::Asterisk:
                return MultiplicativeOperator::Multiplication;
            case tokenizer::Punctuator::Slash:
                return MultiplicativeOperator::Division;
            case tokenizer::Punctuator::Percent:
                return MultiplicativeOperator::Modulo;
            default:
                return std::nullopt;
        }
    }

    AstExpressionPtr ParseMultiplicativeExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        return ParseBinaryExpression<
                MultiplicativeOperator, AstMultiplicativeExpression>(
                GetMultiplicativeOperator, ParseCastExpression, current, end
        );
    }
}// namespace jcc::parsing
