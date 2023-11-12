//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstAndExpression.h"
#include "AstEqualityExpression.h"
#include "../../Parser.h"

namespace parsing {
    std::unique_ptr<AstNode> AstAndExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstEqualityExpression::Parse(parser) };

        if (!left)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            if (token._type != TokenType::Ampersand)
                return left;

            parser.AdvanceCursor();

            std::unique_ptr<AstNode> right{ AstEqualityExpression::Parse(parser) };

            if (!right)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstAndExpression>(std::move(left), std::move(right));
        }
    }

    std::string AstAndExpression::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstAndExpression, depth, {
            TOSTRING_FIELD_NODE("left", *_left)
            TOSTRING_FIELD_NODE("right", *_right)
        })
    }
} // parsing