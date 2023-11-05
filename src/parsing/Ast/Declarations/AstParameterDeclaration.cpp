//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstParameterDeclaration.h"
#include "../../Parser.h"

namespace parsing {
    std::optional<AstParameterDeclaration> AstParameterDeclaration::Parse(Parser &parser) {
        std::optional<AstDeclarationSpecifiers> declSpecifiers{ AstDeclarationSpecifiers::Parse(parser) };
        if (!declSpecifiers)
            return std::nullopt;

        std::optional<AstDeclarator> declarator{ AstDeclarator::Parse(parser) };
        if (!declarator)
            parser.Error(declSpecifiers->_span, "Expected to be followed by a declarator");

        return AstParameterDeclaration { std::move(*declSpecifiers), std::move(*declarator) };
    }

    std::string AstParameterDeclaration::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "ParameterDeclaration {" << std::endl;

        ss << tabsChildren << "declarationSpecifiers: " << _declarationSpecifiers.ToString(depth + 1) << std::endl;
        ss << tabsChildren << "declarator: " << _declarator.ToString(depth + 1) << std::endl;

        ss << tabs << '}';

        return ss.str();
    }
} // parsing