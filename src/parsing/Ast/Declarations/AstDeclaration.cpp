//
// Created by johny on 11/4/23.
//

#include "AstDeclaration.h"
#include "AstStaticAssertDeclaration.h"
#include "../../Parser.h"
#include "AstDeclarationSpecifiers.h"

namespace parsing {
    std::unique_ptr<AstNode> AstDeclaration::Parse(Parser &parser) {
        std::optional<AstStaticAssertDeclaration> staticAssertDeclaration{ AstStaticAssertDeclaration::Parse(parser) };

        if (staticAssertDeclaration)
            return std::make_unique<AstStaticAssertDeclaration>(std::move(*staticAssertDeclaration));

        std::optional<AstDeclarationSpecifiers> declarationSpecifiers{ AstDeclarationSpecifiers::Parse(parser) };

        if (!declarationSpecifiers)
            return nullptr;

        TODO()
    }
} // parsing