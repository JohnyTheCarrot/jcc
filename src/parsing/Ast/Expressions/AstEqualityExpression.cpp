//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstEqualityExpression.h"
#include "AstRelationalExpression.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::unique_ptr<AstNode> AstEqualityExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstRelationalExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            EqualityOperator equalityOperator;

            switch (token._type) {
                case TokenType::Equals:
                    equalityOperator = EqualityOperator::Equal;
                    break;
                case TokenType::NotEquals:
                    equalityOperator = EqualityOperator::NotEqual;
                    break;
                default:
                    return left;
            }

            parser.AdvanceCursor();

            std::unique_ptr<AstNode> right{ AstRelationalExpression::Parse(parser) };

            if (right == nullptr)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstEqualityExpression>(std::move(left), equalityOperator, std::move(right));
        }
    }

    std::string AstEqualityExpression::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstEqualityExpression {" << std::endl;
        ss << tabsChildren << "left: " << _left->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "operator: " << magic_enum::enum_name(_equalityOperator) << std::endl;
        ss << tabsChildren << "right: " << _right->ToString(depth + 1) << std::endl;
        ss << tabs << "}";

        return ss.str();
    }
} // parsing