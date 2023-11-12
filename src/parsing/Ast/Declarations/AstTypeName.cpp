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
        TOSTRING_ONE_FIELD_NODE(AstTypeName, depth, _specifierQualifierList)
    }
} // parsing