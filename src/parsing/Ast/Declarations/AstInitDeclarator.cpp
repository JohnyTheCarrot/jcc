//
// Created by johny on 11/5/23.
//

#include "AstInitDeclarator.h"
#include "../../Parser.h"

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

        TODO()
    }

    std::string AstInitDeclarator::ToString(size_t depth) const {
        TODO()
    }
} // parsing