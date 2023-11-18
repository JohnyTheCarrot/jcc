//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstParameterTypeList.h"
#include "../../Parser.h"

namespace parsing {
    AstNode::Ptr AstParameterTypeList::Parse(Parser &parser) {
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
        TOSTRING_ONE_FIELD_NODE(AstParameterTypeList, depth, _parameterList)
    }
} // parsing