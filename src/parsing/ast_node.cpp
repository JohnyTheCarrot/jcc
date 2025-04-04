#include "ast_node.h"

#include <utility>// for move

#include "parsing/types/type.h"// for ValueType

namespace jcc::parsing {
    AstNode::AstNode(Span span)
        : m_Span{std::move(span)} {
    }

    AstExpression::AstExpression(Span &&span)
        : AstNode{std::move(span)} {
    }

    void AstExpression::AcceptOnNode(AstNodeVisitor *visitor) const {
        AcceptOnExpression(visitor);
    }

    AstBinaryExpression::AstBinaryExpression(
            Span &&span, AstExpressionPtr lhs, AstExpressionPtr rhs,
            mjolnir::Span opSpan
    )
        : AstExpression{std::move(span)}
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

    types::ValueType
    AstBooleanBinaryExpression::GetUsualArithmeticConversionType() const {
        return m_UsualArithmeticConversionType.value();
    }

    void AstBooleanBinaryExpression::SetUsualArithmeticConversionType(
            types::ValueType const &type
    ) const noexcept {
        m_UsualArithmeticConversionType = type;
    }
}// namespace jcc::parsing
