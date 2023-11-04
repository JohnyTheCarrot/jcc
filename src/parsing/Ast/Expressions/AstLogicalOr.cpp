//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstLogicalOr.h"
#include "AstLogicalAnd.h"
#include "../../Parser.h"

namespace parsing {
    std::unique_ptr<AstNode> AstLogicalOr::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstLogicalAnd::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            if (token._type != TokenType::LogicalOr)
                return left;

            parser.AdvanceCursor();

            std::unique_ptr<AstNode> right{ AstLogicalAnd::Parse(parser) };

            if (right == nullptr)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstLogicalOr>(std::move(left), std::move(right));
        }
    }

    std::string AstLogicalOr::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstLogicalOr {" << std::endl;
        ss << tabsChildren << "left: " << _left->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "right: " << _right->ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing