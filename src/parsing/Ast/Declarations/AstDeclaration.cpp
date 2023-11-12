//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstDeclaration.h"
#include "AstStaticAssertDeclaration.h"
#include "../../Parser.h"
#include "AstDeclarationSpecifiers.h"
#include "AstInitDeclaratorList.h"

namespace parsing {
    std::unique_ptr<AstNode> AstDeclaration::Parse(Parser &parser) {
        std::optional<AstStaticAssertDeclaration> staticAssertDeclaration{ AstStaticAssertDeclaration::Parse(parser) };

        if (staticAssertDeclaration)
            return std::make_unique<AstStaticAssertDeclaration>(std::move(*staticAssertDeclaration));

        std::optional<AstDeclarationSpecifiers> declarationSpecifiers{ AstDeclarationSpecifiers::Parse(parser) };

        if (!declarationSpecifiers)
            return nullptr;

        std::optional<AstInitDeclaratorList> initDeclaratorList{ AstInitDeclaratorList::Parse(parser) };
        parser.ExpectToken(TokenType::Semicolon);

        return std::make_unique<AstDeclaration>(std::move(*declarationSpecifiers), std::move(initDeclaratorList));
    }

    std::string AstDeclaration::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstDeclaration, depth, {
            TOSTRING_FIELD_NODE("declarationSpecifiers", _declarationSpecifiers)
            if (_initDeclaratorList)
                TOSTRING_FIELD_NODE("initDeclaratorList", *_initDeclaratorList)
        })
    }
} // parsing