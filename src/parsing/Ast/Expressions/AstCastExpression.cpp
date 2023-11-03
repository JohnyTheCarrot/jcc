//
// Created by johny on 11/3/23.
//

#include <sstream>
#include "AstCastExpression.h"
#include "../../Parser.h"
#include "AstUnaryExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstCastExpression::Parse(Parser &parser) {
        if (!parser)
            return nullptr;

        int cursor{ parser.GetCursor() };

        std::optional<Token> lParen{ parser.ConsumeIfTokenIs(TokenType::LeftParenthesis) };
        if (!lParen)
            return AstUnaryExpression::Parse(parser);

        std::optional<AstTypeName> typeName{ AstTypeName::Parse(parser) };
        if (!typeName) {
            parser.SetCursor(cursor);
            return AstUnaryExpression::Parse(parser);
        }

        std::optional<Token> rParen{parser.ConsumeIfTokenIs(TokenType::RightParenthesis) };
        if (!rParen) {
            parser.Error(parser.PeekNextToken()._span, "Expected ')'");
        }

        std::unique_ptr<AstNode> expression{ AstCastExpression::Parse(parser) };

        // if the type cast is not followed by an expression, it is not a type cast
        if (!expression) {
            parser.SetCursor(cursor);
            return AstUnaryExpression::Parse(parser);
        }

        Span typeNameSpan{ typeName->_span };
        Span expressionSpan{ expression->_span };

        std::unique_ptr<AstNode> castExpression{ std::make_unique<AstCastExpression>(std::move(typeName), std::move(expression)) };
        castExpression->_span = lParen->_span + typeNameSpan + rParen->_span + expressionSpan;

        return castExpression;
    }

    std::string AstCastExpression::ToString(size_t depth) const {
        // _typeName won't ever be nullopt given if the cast expression node exists, it must have a type name
        std::string tabs = Indent(depth);
        std::string childTabs = Indent(depth + 1);

        std::stringstream ss;
        ss << "CastExpression {" << std::endl;
        ss << childTabs << "typeName: " << _typeName->ToString(depth + 1) << std::endl;
        ss << childTabs << "expression: " << _expression->ToString(depth + 1) << std::endl;
        ss << tabs << "}";

        return ss.str();
    }
} // parsing