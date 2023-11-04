//
// Created by johny on 11/2/23.
//

#include "AstExpression.h"
#include "AstEqualityExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstExpression::Parse(Parser &parser) {
        return AstEqualityExpression::Parse(parser);
    }
} // parsing