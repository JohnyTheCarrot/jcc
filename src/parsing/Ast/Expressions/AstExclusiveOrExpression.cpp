//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstExclusiveOrExpression.h"
#include "AstAndExpression.h"
#include "../../Parser.h"

namespace parsing {
    AstNode::Ptr AstExclusiveOrExpression::Parse(Parser &parser) {
        AstNode::Ptr left{ AstAndExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            if (token._type != TokenType::ExclusiveOr)
                return left;

            parser.AdvanceCursor();

            AstNode::Ptr right{ AstAndExpression::Parse(parser) };

            if (right == nullptr)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstExclusiveOrExpression>(std::move(left), std::move(right));
        }
    }

    std::string AstExclusiveOrExpression::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstExclusiveOrExpression, depth, {
            TOSTRING_FIELD_NODE("left", *_left)
            TOSTRING_FIELD_NODE("right", *_right)
        })

    }
} // parsing