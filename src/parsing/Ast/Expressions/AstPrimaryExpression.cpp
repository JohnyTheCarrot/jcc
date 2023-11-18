//
// Created by johny on 10/19/23.
//

#include "AstPrimaryExpression.h"
#include "AstNumericalConstantExpression.h"
#include "../../Parser.h"
#include "AstExpression.h"
#include "AstStringLiteralExpression.h"

namespace parsing {
    AstNode::Ptr AstPrimaryExpression::Parse(Parser &parser) {
        std::optional<AstNumericalConstantExpression> constant = AstNumericalConstantExpression::Parse(parser);
        if (constant) {
            return std::make_unique<AstNumericalConstantExpression>(*constant);
        }

        std::optional<AstIdentifierExpression> identifier = AstIdentifierExpression::Parse(parser);
        if (identifier) {
            return std::make_unique<AstIdentifierExpression>(*identifier);
        }

        std::optional<AstStringLiteralExpression> stringLiteral = AstStringLiteralExpression::Parse(parser);
        if (stringLiteral) {
            return std::make_unique<AstStringLiteralExpression>(*stringLiteral);
        }

        int parserCursor{ parser.GetCursor() };
        std::optional<Token> leftParen{ parser.ConsumeIfTokenIs(TokenType::LeftParenthesis) };

        if (leftParen.has_value()) {
           AstNode::Ptr expression{ AstExpression::Parse(parser) };

           if (expression == nullptr) {
               parser.Error(leftParen->_span, "Expected expression");
           } else if (!parser.AdvanceIfTokenIs(TokenType::RightParenthesis)) {
               parser.Error(expression->_span, "Expected to be followed by ')'");
           }

           return expression;
        }

        parser.SetCursor(parserCursor);
        return nullptr;
    }

    std::string AstPrimaryExpression::ToString(size_t depth) const {
        NOT_APPLICABLE()
    }
} // parsing