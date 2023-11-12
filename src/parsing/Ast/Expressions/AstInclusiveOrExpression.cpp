//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstInclusiveOrExpression.h"
#include "AstExclusiveOrExpression.h"
#include "../../Parser.h"

namespace parsing {
    std::unique_ptr<AstNode> AstInclusiveOrExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{AstExclusiveOrExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            if (token._type != TokenType::BitwiseOr)
                return left;

            parser.AdvanceCursor();

            std::unique_ptr<AstNode> right{AstExclusiveOrExpression::Parse(parser) };

            if (right == nullptr)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstInclusiveOrExpression>(std::move(left), std::move(right));
        }
    }

    std::string AstInclusiveOrExpression::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstInclusiveOrExpression, depth, {
            TOSTRING_FIELD_NODE("left", *_left)
            TOSTRING_FIELD_NODE("right", *_right)
        })
    }
} // parsing