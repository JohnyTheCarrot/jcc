//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstParameterList.h"
#include "AstParameterDeclaration.h"
#include "../../Parser.h"

namespace parsing {
    std::optional<AstParameterList> AstParameterList::Parse(Parser &parser) {
        std::vector<AstParameterDeclaration> declarations{};
        std::optional<AstParameterDeclaration> paramDecl{ AstParameterDeclaration::Parse(parser) };

        if (!paramDecl)
            return std::nullopt;

        declarations.push_back(std::move(*paramDecl));

        while (true) {
            int currentCursor{ parser.GetCursor() };
            std::optional<Token> comma{ parser.ConsumeIfTokenIs(TokenType::Comma) };

            if (!comma)
                break;

            paramDecl = AstParameterDeclaration::Parse(parser);
            if (!paramDecl) {
                parser.SetCursor(currentCursor);
                break;
            }

            declarations.push_back(std::move(*paramDecl));
        }

        return AstParameterList{ std::move(declarations) };
    }

    std::string AstParameterList::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "ParameterList([" << std::endl;

        for (const auto &paramDeclaration: _paramDeclarations) {
            ss << tabsChildren << paramDeclaration.ToString(depth + 1) << std::endl;
        }

        ss << tabs << "])";

        return ss.str();
    }
} // parsing