//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstLogicalOrExpression.h"
#include "AstLogicalAndExpression.h"
#include "../../Parser.h"

namespace parsing {
    std::unique_ptr<AstNode> AstLogicalOrExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{AstLogicalAndExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            if (token._type != TokenType::LogicalOr)
                return left;

            parser.AdvanceCursor();

            std::unique_ptr<AstNode> right{AstLogicalAndExpression::Parse(parser) };

            if (right == nullptr)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstLogicalOrExpression>(std::move(left), std::move(right));
        }
    }

    std::string AstLogicalOrExpression::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstLogicalOrExpression, depth, {
            TOSTRING_FIELD_NODE("left", *_left)
            TOSTRING_FIELD_NODE("right", *_right)
        })
    }
} // parsing