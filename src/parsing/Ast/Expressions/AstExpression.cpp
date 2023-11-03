//
// Created by johny on 11/2/23.
//

#include "AstExpression.h"
#include "AstMultiplicativeExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstExpression::Parse(Parser &parser) {
        return AstMultiplicativeExpression::Parse(parser);
    }
} // parsing