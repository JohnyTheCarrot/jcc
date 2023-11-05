//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstInitDeclaratorList.h"
#include "../../Parser.h"
#include "AstInitDeclarator.h"

namespace parsing {
    std::optional<AstInitDeclaratorList> AstInitDeclaratorList::Parse(Parser &parser) {
        std::vector<std::unique_ptr<AstNode>> list{};
        std::unique_ptr<AstNode> initDecl{ AstInitDeclarator::Parse(parser) };

        if (!initDecl) {
            return std::nullopt;
        }

        list.push_back(std::move(initDecl));

        while (true) {
            std::optional<Token> comma{ parser.ConsumeIfTokenIs(TokenType::Comma) };

            if (!comma) {
                break;
            }

            std::unique_ptr<AstNode> nextInitDecl{ AstInitDeclarator::Parse(parser) };
            if (!nextInitDecl)
                parser.Error("Expected init declarator after comma");

            list.push_back(std::move(nextInitDecl));
        }

        return AstInitDeclaratorList{ std::move(list) };
    }

    std::string AstInitDeclaratorList::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstInitDeclaratorList([" << std::endl;

        for (const auto &initDecl : _initDeclaratorList) {
            ss << tabsChildren << initDecl->ToString(depth + 1) << std::endl;
        }

        ss << tabs << "])";

        return ss.str();
    }
} // parsing