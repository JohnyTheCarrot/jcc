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
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstAndExpression {" << std::endl;
        ss << tabsChildren << "left: " << _left->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "right: " << _right->ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing