#include "ast_node.h"

#include "parser.h"

namespace jcc::parsing_sema {
    AstNode::AstNode(Span &&span)
        : m_Span{std::move(span)} {
    }

    AstExpression::AstExpression(Span &&span, types::ValueType type)
        : AstNode{std::move(span)}
        , m_Type{std::move(type)} {
    }

    types::ValueType const &AstExpression::GetType() const {
        return m_Type;
    }

    AstBinaryExpression::AstBinaryExpression(
            Span &&span, AstExpressionPtr lhs, AstExpressionPtr rhs,
            mjolnir::Span opSpan
    )
        : AstExpression{std::move(span), UsualArithmeticConversions(lhs->GetType(), rhs->GetType())}
        , m_Lhs{std::move(lhs)}
        , m_Rhs{std::move(rhs)}
        , m_OpSpan{opSpan} {
    }

    AstExpressionPtr::pointer AstBinaryExpression::GetLhs() const noexcept {
        return m_Lhs.get();
    }

    AstExpressionPtr::pointer AstBinaryExpression::GetRhs() const noexcept {
        return m_Rhs.get();
    }

    mjolnir::Span AstBinaryExpression::GetOpSpan() const noexcept {
        return m_OpSpan;
    }
}// namespace jcc::parsing_sema
