//
// Created by johny on 10/19/23.
//

#include "AstPrimaryExpression.h"
#include "AstConstantExpression.h"
#include "../../Parser.h"
#include "AstExpression.h"
#include "AstStringLiteralExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstPrimaryExpression::Parse(Parser &parser) {
        std::optional<AstConstantExpression> constant = AstConstantExpression::Parse(parser);
        if (constant) {
            return std::make_unique<AstConstantExpression>(*constant);
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
           std::unique_ptr<AstNode> expression{ AstExpression::Parse(parser) };

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
} // parsing