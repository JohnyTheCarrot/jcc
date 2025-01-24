#include "ast_node.h"

#include "parser.h"

namespace jcc::parsing_sema {
    AstExpression::AstExpression(types::ValueType type)
        : m_Type{std::move(type)} {
    }

    types::ValueType const &AstExpression::GetType() const {
        return m_Type;
    }
}// namespace jcc::parsing_sema
