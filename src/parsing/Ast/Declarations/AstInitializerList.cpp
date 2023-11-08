//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstInitializerList.h"
#include "../../Parser.h"
#include "AstInitializer.h"

struct Mat {
    bool isConfused;
} mat {
    .isConfused = true
};

namespace parsing {
    std::optional<AstInitializerList> AstInitializerList::Parse(Parser &parser) {
        std::vector<InitializerListItem> initializerListItems{};
        Span span;
        bool isFirstIteration{ true };
        int cursor{ parser.GetCursor() };

        while (true) {
            std::optional<AstDesignation> designation{ AstDesignation::Parse(parser) };
            std::unique_ptr<AstNode> initializer{ AstInitializer::Parse(parser) };

            if (!initializer) {
                if (designation)
                    parser.Error(designation->_span, "Expected to be followed by initializer");

                if (isFirstIteration)
                    return std::nullopt;

                parser.SetCursor(cursor);
                break;
            }

            isFirstIteration = false;

            span += designation->_span + initializer->_span;
            initializerListItems.emplace_back(std::move(designation), std::move(initializer));

            cursor = parser.GetCursor();
            parser.AdvanceIfTokenIs(TokenType::Comma);
        }

        return AstInitializerList{ span, std::move(initializerListItems) };
    }

    std::string AstInitializerList::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };
        std::string tabsChildren2{ Indent(depth + 2) };

        ss << "InitializerList([";
        for (const auto &listItem: _listItems) {
            ss << std::endl;
            ss << tabsChildren << '{' << std::endl;
            if (listItem._designation)
                ss << tabsChildren2 << "designation: " << listItem._designation->ToString(depth + 2) << std::endl;

            ss << tabsChildren2 << "initializer: " << listItem._initializer->ToString(depth + 2) << std::endl;
            ss << tabsChildren << '}';
        }

        ss << std::endl << tabs << "])";

        return ss.str();
    }
} // parsing