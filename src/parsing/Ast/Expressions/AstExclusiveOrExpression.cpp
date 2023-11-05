//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstExclusiveOrExpression.h"
#include "AstAndExpression.h"
#include "../../Parser.h"

namespace parsing {
    std::unique_ptr<AstNode> AstExclusiveOrExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstAndExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        while (true) {
            if (!parser)
                return left;

            const Token &token{ parser.PeekNextToken() };

            if (token._type != TokenType::ExclusiveOr)
                return left;

            parser.AdvanceCursor();

            std::unique_ptr<AstNode> right{ AstAndExpression::Parse(parser) };

            if (right == nullptr)
                parser.Error(token._span, "Expected rhs expression");

            left = std::make_unique<AstExclusiveOrExpression>(std::move(left), std::move(right));
        }
    }

    std::string AstExclusiveOrExpression::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstExclusiveOrExpression {" << std::endl;
        ss << tabsChildren << "left: " << _left->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "right: " << _right->ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing