//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstInitDeclaratorList.h"
#include "../../Parser.h"
#include "AstInitDeclarator.h"

namespace parsing {
    std::optional<AstInitDeclaratorList> AstInitDeclaratorList::Parse(Parser &parser) {
        std::vector<AstNode::Ptr> list{};
        AstNode::Ptr initDecl{ AstInitDeclarator::Parse(parser) };

        if (!initDecl) {
            return std::nullopt;
        }

        list.push_back(std::move(initDecl));

        while (true) {
            std::optional<Token> comma{ parser.ConsumeIfTokenIs(TokenType::Comma) };

            if (!comma) {
                break;
            }

            AstNode::Ptr nextInitDecl{ AstInitDeclarator::Parse(parser) };
            if (!nextInitDecl)
                parser.Error("Expected init declarator after comma");

            list.push_back(std::move(nextInitDecl));
        }

        return AstInitDeclaratorList{ std::move(list) };
    }

    std::string AstInitDeclaratorList::ToString(size_t depth) const {
        TOSTRING_LIST(AstInitDeclaratorList, depth, {
            for (const auto &initDecl : _initDeclaratorList) {
                TOSTRING_LIST_ITEM_NODE(*initDecl)
            }
        })
    }
} // parsing