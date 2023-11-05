//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstTypeQualifierList.h"

namespace parsing {
    std::optional<AstTypeQualifierList> AstTypeQualifierList::Parse(Parser &parser) {
        std::vector<AstTypeQualifier> list;
        Span span;

        if (auto typeQualifier{ AstTypeQualifier::Parse(parser) }) {
            span = typeQualifier->_span;
            list.push_back(std::move(*typeQualifier));
        } else
            return std::nullopt;

        while (true) {
            if (auto typeQualifier{ AstTypeQualifier::Parse(parser) }) {
                span += typeQualifier->_span;
                list.push_back(std::move(*typeQualifier));
            } else {
                break;
            }
        }

        return AstTypeQualifierList{ span, std::move(list) };
    }

    std::string AstTypeQualifierList::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "TypeQualifierList([" << std::endl;

        for (const auto &typeQualifier : _list) {
            ss << tabsChildren << typeQualifier.ToString(depth + 1) << std::endl;
        }

        ss << tabs << "])";

        return ss.str();
    }
} // parsing