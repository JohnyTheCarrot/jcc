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
}// namespace jcc::parsing_sema
