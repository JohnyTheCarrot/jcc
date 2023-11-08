//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstInitDeclarator.h"
#include "../../Parser.h"
#include "AstInitializer.h"

namespace parsing {
    std::unique_ptr<AstNode> AstInitDeclarator::Parse(Parser &parser) {
        std::optional<AstDeclarator> declarator{ AstDeclarator::Parse(parser) };

        if (!declarator) {
            return nullptr;
        }

        std::optional<Token> assignmentOperator{ parser.ConsumeIfTokenIs(TokenType::Assign) };
        if (!assignmentOperator) {
            return std::make_unique<AstDeclarator>(std::move(*declarator));
        }

        std::unique_ptr<AstNode> initializer{ AstInitializer::Parse(parser) };
        if (!initializer)
            parser.Error(assignmentOperator->_span, "Expected to be followed by an initializer");

        return std::make_unique<AstInitDeclarator>(
                declarator->_span + assignmentOperator->_span + initializer->_span,
                std::move(*declarator),
                std::move(initializer)
        );
    }

    std::string AstInitDeclarator::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{Indent(depth + 1) };

        ss << "InitDeclarator {" << std::endl;
        ss << tabsChildren << "declarator: " << _declarator.ToString(depth + 1) << std::endl;

        if (_initializer) {
            ss << tabsChildren << "initializer: " << _initializer->ToString(depth + 1) << std::endl;
        }

        ss << tabs << '}';

        return ss.str();
    }
} // parsing