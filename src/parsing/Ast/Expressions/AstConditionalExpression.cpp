//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstConditionalExpression.h"
#include "AstLogicalOrExpression.h"
#include "../../Parser.h"
#include "AstExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstConditionalExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> condition{ AstLogicalOrExpression::Parse(parser) };

        if (condition == nullptr)
            return nullptr;

        if (!parser)
            return condition;

        std::optional<Token> questionMark{parser.ConsumeIfTokenIs(TokenType::QuestionMark) };

        if (!questionMark.has_value())
            return condition;

        std::unique_ptr<AstNode> trueExpression{ AstExpression::Parse(parser) };

        if (trueExpression == nullptr || !parser) {
            parser.Error(questionMark->_span, "Expected expression for true branch");
        }

        std::optional<Token> colon{ parser.ConsumeIfTokenIs(TokenType::Colon) };

        std::unique_ptr<AstNode> falseExpression{ AstConditionalExpression::Parse(parser) };

        if (falseExpression == nullptr) {
            parser.Error(colon->_span, "Expected expression for false branch");
        }

        return std::make_unique<AstConditionalExpression>(std::move(condition), std::move(trueExpression), std::move(falseExpression));
    }

    std::string AstConditionalExpression::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstConditionalExpression {" << std::endl;
        ss << tabsChildren << "condition: " << _condition->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "trueExpression: " << _trueExpression->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "falseExpression: " << _falseExpression->ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing