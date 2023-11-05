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
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "Declaration {" << std::endl;
        ss << tabsChildren << "declarationSpecifiers: " << _declarationSpecifiers.ToString(depth + 1) << std::endl;

        if (_initDeclaratorList)
            ss << tabsChildren << "initDeclaratorList: " << _initDeclaratorList->ToString(depth + 1) << std::endl;

        ss << tabs << '}';

        return ss.str();
    }
} // parsing