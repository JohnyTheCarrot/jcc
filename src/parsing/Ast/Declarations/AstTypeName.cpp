//
// Created by johny on 11/3/23.
//

#include <sstream>
#include "AstTypeName.h"
#include "../../../tokenizer.h"
#include "../../Parser.h"

namespace parsing {
    std::optional<AstTypeName> AstTypeName::Parse(Parser &parser) {
        std::optional<AstSpecifierQualifierList> list{ AstSpecifierQualifierList::Parse(parser) };

        if (!list.has_value()) {
            return std::nullopt;
        }

        return AstTypeName{ *list };
    }

    std::string AstTypeName::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstTypeName(" << std::endl;
        ss << tabsChildren << _specifierQualifierList.ToString(depth + 1) << std::endl;
        ss << tabs << ')';

        return ss.str();
    }
} // parsing