//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstInitDeclarator.h"
#include "../../Parser.h"
#include "AstInitializer.h"

namespace parsing {
    AstNode::Ptr AstInitDeclarator::Parse(Parser &parser) {
        std::optional<AstDeclarator> declarator{ AstDeclarator::Parse(parser) };

        if (!declarator) {
            return nullptr;
        }

        std::optional<Token> assignmentOperator{ parser.ConsumeIfTokenIs(TokenType::Assign) };
        if (!assignmentOperator) {
            return std::make_unique<AstDeclarator>(std::move(*declarator));
        }

        AstNode::Ptr initializer{ AstInitializer::Parse(parser) };
        if (!initializer)
            parser.Error(assignmentOperator->_span, "Expected to be followed by an initializer");

        return std::make_unique<AstInitDeclarator>(
                declarator->_span + assignmentOperator->_span + initializer->_span,
                std::move(*declarator),
                std::move(initializer)
        );
    }

    std::string AstInitDeclarator::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstInitDeclarator, depth, {
            TOSTRING_FIELD_NODE("declarator", _declarator)

            if (_initializer)
                TOSTRING_FIELD_NODE("initializer", *_initializer)
        })
    }
} // parsing