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
        TOSTRING_FIELDS(AstConditionalExpression, depth, {
            TOSTRING_FIELD_NODE("condition", *_condition)
            TOSTRING_FIELD_NODE("trueExpression", *_trueExpression)
            TOSTRING_FIELD_NODE("falseExpression", *_falseExpression)
        })
    }
} // parsing