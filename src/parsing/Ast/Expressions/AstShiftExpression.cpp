//
// Created by johny on 11/3/23.
//

#include <sstream>
#include "AstShiftExpression.h"
#include "AstAdditiveExpression.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::unique_ptr<AstNode> AstShiftExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstAdditiveExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            ShiftOperator shiftOperator;

            switch (token._type) {
                case TokenType::ShiftLeft:
                    shiftOperator = ShiftOperator::ShiftLeft;
                    break;
                case TokenType::ShiftRight:
                    shiftOperator = ShiftOperator::ShiftRight;
                    break;
                default:
                    return left;
            }

            parser.AdvanceCursor();

            std::unique_ptr<AstNode> right{ AstAdditiveExpression::Parse(parser) };

            if (right == nullptr)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstShiftExpression>(std::move(left), shiftOperator, std::move(right));
        }
    }

    std::string AstShiftExpression::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstShiftExpression, depth, {
            TOSTRING_FIELD_NODE("left", *_left)
            TOSTRING_FIELD_ENUM("operator", _shiftOperator)
            TOSTRING_FIELD_NODE("right", *_right)
        })
    }
} // parsing