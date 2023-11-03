//
// Created by johny on 10/19/23.
//

#include "AstPrimaryExpression.h"
#include "AstConstant.h"
#include "../../Parser.h"
#include "AstExpression.h"
#include "AstStringLiteral.h"

namespace parsing {
    std::unique_ptr<AstNode> AstPrimaryExpression::Parse(Parser &parser) {
        std::optional<AstConstant> constant = AstConstant::Parse(parser);
        if (constant) {
            return std::make_unique<AstConstant>(*constant);
        }

        std::optional<AstIdentifier> identifier = AstIdentifier::Parse(parser);
        if (identifier) {
            return std::make_unique<AstIdentifier>(*identifier);
        }

        std::optional<AstStringLiteral> stringLiteral = AstStringLiteral::Parse(parser);
        if (stringLiteral) {
            return std::make_unique<AstStringLiteral>(*stringLiteral);
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