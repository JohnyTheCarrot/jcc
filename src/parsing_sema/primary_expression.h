#ifndef PRIMARY_EXPRESSION_H
#define PRIMARY_EXPRESSION_H

#include "ast_node.h"

namespace jcc::tokenizer {
    struct Token;
}

namespace jcc::parsing_sema {
    [[nodiscard]]
    AstExpressionPtr ParsePrimaryExpression(tokenizer::Token &token);
}

#endif//PRIMARY_EXPRESSION_H
