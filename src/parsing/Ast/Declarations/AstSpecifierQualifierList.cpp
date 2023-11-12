//
// Created by johny on 11/3/23.
//

#include <sstream>
#include "AstSpecifierQualifierList.h"
#include "../../../tokenizer.h"
#include "../../Parser.h"

namespace parsing {
    std::optional<AstSpecifierQualifierList> AstSpecifierQualifierList::Parse(Parser &parser) {
        AstSpecifierQualifierList specifierQualifierListNode{ };

        while (true) {
            int parserCursor{ parser.GetCursor() };

            std::optional<AstTypeSpecifier> typeSpecifier{ AstTypeSpecifier::Parse(parser) };
            if (typeSpecifier.has_value()) {
                specifierQualifierListNode._list.emplace_back(*typeSpecifier);
                continue;
            }

            std::optional<AstTypeQualifier> typeQualifier{ AstTypeQualifier::Parse(parser) };
            if (typeQualifier.has_value()) {
                specifierQualifierListNode._list.emplace_back(*typeQualifier);
                continue;
            }

            parser.SetCursor(parserCursor);
            break;
        }

        if (specifierQualifierListNode._list.empty())
            return std::nullopt;

        return specifierQualifierListNode;
    }

    std::string AstSpecifierQualifierList::ToString(size_t depth) const {
        TOSTRING_LIST(AstSpecifierQualifierList, depth, {
            for (const auto &item : this->_list) {
                if (std::holds_alternative<AstTypeSpecifier>(item))
                    TOSTRING_LIST_ITEM_NODE(std::get<AstTypeSpecifier>(item))
                else if (std::holds_alternative<AstTypeQualifier>(item))
                    TOSTRING_LIST_ITEM_NODE(std::get<AstTypeQualifier>(item))
                else
                    TODO()
            }
        })
    }
} // parsing