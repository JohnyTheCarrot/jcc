//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstDeclarator.h"
#include "../../Parser.h"
#include "AstDirectDeclarator.h"

namespace parsing {
    std::optional<AstDeclarator> AstDeclarator::Parse(Parser &parser) {
        std::optional<AstPointer> pointer{ AstPointer::Parse(parser) };
        std::unique_ptr<AstNode> directDeclarator{ AstDirectDeclarator::Parse(parser) };

        if (!directDeclarator) {
            if (pointer)
                parser.Error(pointer->_span, "Expected direct declarator after pointer");

            return std::nullopt;
        }

        Span span;
        if (pointer) {
            span = pointer->_span + directDeclarator->_span;
        } else {
            span = directDeclarator->_span;
        }

        AstDeclarator declarator{ std::move(pointer), std::move(directDeclarator) };
        declarator._span = span;
        return declarator;
    }

    std::string AstDeclarator::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstDeclarator {" << std::endl;

        if (_pointer) {
            ss << tabsChildren << "pointer: " << _pointer->ToString(depth + 1) << std::endl;
        }

        ss << tabsChildren << "directDeclarator: " << _directDeclarator->ToString(depth + 1) << std::endl;

        ss << tabs << '}';

        return ss.str();
    }
} // parsing