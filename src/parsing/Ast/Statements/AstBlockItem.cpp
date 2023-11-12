//
// Created by johny on 11/8/23.
//

#include "AstBlockItem.h"
#include "../Declarations/AstDeclaration.h"
#include "AstStatement.h"

namespace parsing {
    std::unique_ptr<AstNode> AstBlockItem::Parse(Parser &parser) {
        std::unique_ptr<AstNode> declaration{ AstDeclaration::Parse(parser) };
        if (declaration)
            return declaration;

        return AstStatement::Parse(parser);
    }

    std::string AstBlockItem::ToString(size_t depth) const {
        NOT_APPLICABLE()
    }
} // parsing