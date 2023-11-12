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
        TOSTRING_FIELDS(AstParameterDeclaration, depth, {
            TOSTRING_FIELD_NODE("declarationSpecifiers", _declarationSpecifiers)
            TOSTRING_FIELD_NODE("declarator", _declarator)
        })
    }
} // parsing