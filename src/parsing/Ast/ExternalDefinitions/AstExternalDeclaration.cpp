//
// Created by johny on 11/12/23.
//

#include "AstExternalDeclaration.h"
#include "../../Parser.h"
#include "AstFunctionDefinition.h"
#include "../Declarations/AstDeclaration.h"

namespace parsing {
    std::unique_ptr<AstNode> AstExternalDeclaration::Parse(Parser &parser) {
        std::optional<AstFunctionDefinition> functionDefinition{ AstFunctionDefinition::Parse(parser) };
        if (functionDefinition)
            return std::make_unique<AstFunctionDefinition>(std::move(*functionDefinition));

        return AstDeclaration::Parse(parser);
    }

    std::string AstExternalDeclaration::ToString(size_t depth) const {
        NOT_APPLICABLE()
    }
} // parsing