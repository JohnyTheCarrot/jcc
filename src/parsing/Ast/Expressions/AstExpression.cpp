//
// Created by johny on 11/2/23.
//

#include "AstExpression.h"
#include "AstRelationalExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstExpression::Parse(Parser &parser) {
        return AstRelationalExpression::Parse(parser);
    }
} // parsing