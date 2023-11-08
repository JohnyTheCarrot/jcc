//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstDesignation.h"
#include "../../Parser.h"

namespace parsing {
    std::optional<AstDesignation> AstDesignation::Parse(Parser &parser) {
        std::optional<AstDesignatorList> designatorList{ AstDesignatorList::Parse(parser) };
        if (!designatorList)
            return std::nullopt;

        const Token &equalsToken{ parser.ExpectToken(TokenType::Assign) };

        return AstDesignation{ designatorList->_span + equalsToken._span, std::move(*designatorList) };
    }

    std::string AstDesignation::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstDesignation {" << std::endl;
        ss << tabsChildren << "designatorList: " << _designatorList.ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing