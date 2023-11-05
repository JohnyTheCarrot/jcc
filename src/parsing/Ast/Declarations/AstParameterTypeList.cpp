//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstParameterTypeList.h"
#include "../../Parser.h"

namespace parsing {
    std::unique_ptr<AstNode> AstParameterTypeList::Parse(Parser &parser) {
        std::optional<AstParameterList> parameterList{ AstParameterList::Parse(parser) };
        if (!parameterList)
            return nullptr;

        std::optional<Token> comma{ parser.ConsumeIfTokenIs(TokenType::Comma) };
        if (!comma)
            return std::make_unique<AstParameterList>(std::move(*parameterList));

        parser.ExpectToken(TokenType::Ellipsis);

        return std::make_unique<AstParameterTypeList>(std::move(*parameterList));
    }

    std::string AstParameterTypeList::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "ParameterTypeList {" << std::endl;
        ss << tabsChildren << _parameterList.ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing