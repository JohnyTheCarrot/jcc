//
// Created by johny on 11/3/23.
//

#include <sstream>
#include "AstAdditiveExpression.h"
#include "../../Parser.h"
#include "AstMultiplicativeExpression.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::unique_ptr<AstNode> AstAdditiveExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstMultiplicativeExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            AdditiveOperator additiveOperator;

            switch (token._type) {
                case TokenType::Plus:
                    additiveOperator = AdditiveOperator::Plus;
                    break;
                case TokenType::Minus:
                    additiveOperator = AdditiveOperator::Minus;
                    break;
                default:
                    return left;
            }

            parser.AdvanceCursor();

            std::unique_ptr<AstNode> right{ AstMultiplicativeExpression::Parse(parser) };

            if (right == nullptr)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstAdditiveExpression>(std::move(left), additiveOperator, std::move(right));
        }
    }

    std::string AstAdditiveExpression::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstAdditiveExpression {" << std::endl;
        ss << tabsChildren << "left: " << _left->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "operator: " << magic_enum::enum_name(_additiveOperator) << std::endl;
        ss << tabsChildren << "right: " << _right->ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing