//
// Created by johny on 11/2/23.
//

#include <sstream>
#include "AstExpression.h"
#include "AstAssignmentExpression.h"
#include "../../Parser.h"

namespace parsing {
    std::unique_ptr<AstNode> AstExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstAssignmentExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        std::vector<std::unique_ptr<AstNode>> expressions{ };
        expressions.push_back(nullptr);

        while (true) {
            if (!parser)
                break;

            std::optional<Token> token{ parser.ConsumeIfTokenIs(TokenType::Comma) };

            if (!token.has_value())
                break;

            std::unique_ptr<AstNode> right{ AstAssignmentExpression::Parse(parser) };

            if (right == nullptr)
                parser.Error(token->_span, "Expected expression");

            expressions.push_back(std::move(right));
        }

        if (expressions.size() == 1)
            return left;

        expressions[0] = std::move(left);

        return std::make_unique<AstExpression>(std::move(expressions));
    }

    std::string AstExpression::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstExpression([" << std::endl;

        for (const std::unique_ptr<AstNode> &expression : _expressions) {
            ss << tabsChildren << expression->ToString(depth + 1) << std::endl;
        }

        ss << tabs << "])";

        return ss.str();
    }
} // parsing