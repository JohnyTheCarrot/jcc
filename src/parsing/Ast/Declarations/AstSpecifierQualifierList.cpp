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
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstSpecifierQualifierList([" << std::endl;

        for (const auto &item: this->_list) {
            ss << tabsChildren;
            if (std::holds_alternative<AstTypeSpecifier>(item)) {
                ss << std::get<AstTypeSpecifier>(item).ToString(depth + 1) << ',' << std::endl;
            } else if (std::holds_alternative<AstTypeQualifier>(item)) {
                ss << std::get<AstTypeQualifier>(item).ToString(depth + 1) << ',' << std::endl;
            } else
                TODO()
        }

        ss << tabs << "])";

        return ss.str();
    }
} // parsing