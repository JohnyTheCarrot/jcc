//
// Created by johny on 11/2/23.
//

#include "AstExpression.h"
#include "AstUnaryExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstExpression::Parse(Parser &parser) {
        return AstUnaryExpression::Parse(parser);
    }
} // parsing