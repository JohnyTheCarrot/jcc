//
// Created by johny on 11/4/23.
//

#include "AstConstantExpression.h"
#include "AstConditionalExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstConstantExpression::Parse(Parser &parser) {
        return AstConditionalExpression::Parse(parser);
    }

    std::string AstConstantExpression::ToString(size_t depth) const {
        TODO()
    }
} // parsing