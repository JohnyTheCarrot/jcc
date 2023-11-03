//
// Created by johny on 11/3/23.
//

#include <sstream>
#include "AstMultiplicativeExpression.h"
#include "AstCastExpression.h"
#include "../../../tokenizer.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::unique_ptr<AstNode> AstMultiplicativeExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstCastExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            MultiplicativeOperator multiplicativeOperator;

            switch (token._type) {
                case TokenType::Asterisk:
                    multiplicativeOperator = MultiplicativeOperator::Multiply;
                    break;
                case TokenType::Divide:
                    multiplicativeOperator = MultiplicativeOperator::Divide;
                    break;
                case TokenType::Modulo:
                    multiplicativeOperator = MultiplicativeOperator::Modulo;
                    break;
                default:
                    return left;
            }

            parser.AdvanceCursor();

            std::unique_ptr<AstNode> right{ AstCastExpression::Parse(parser) };

            if (right == nullptr)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstMultiplicativeExpression>(std::move(left), multiplicativeOperator, std::move(right));
        }
    }

    std::string AstMultiplicativeExpression::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstMultiplicativeExpression {" << std::endl;
        ss << tabsChildren << "left: " << _left->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "operator: " << magic_enum::enum_name(_multiplicativeOperator) << std::endl;
        ss << tabsChildren << "right: " << _right->ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing