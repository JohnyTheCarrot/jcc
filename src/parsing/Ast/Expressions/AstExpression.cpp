//
// Created by johny on 11/2/23.
//

#include "AstExpression.h"
#include "AstConditionalExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstExpression::Parse(Parser &parser) {
        return AstConditionalExpression::Parse(parser);
    }
} // parsing