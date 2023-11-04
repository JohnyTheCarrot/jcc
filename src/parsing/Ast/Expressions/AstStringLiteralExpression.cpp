//
// Created by johny on 11/3/23.
//

#include "AstStringLiteralExpression.h"
#include "../../Parser.h"

namespace parsing {
    std::string AstStringLiteralExpression::ToString(size_t depth) const {
        return "AstStringLiteralExpression(\"" + _string + "\")";
    }

    std::optional<AstStringLiteralExpression> AstStringLiteralExpression::Parse(Parser &parser) {
        int parserCursor{ parser.GetCursor() };

        std::optional<Token> stringLiteral{ parser.ConsumeIfTokenIs(TokenType::StringLiteral) };

        if (!stringLiteral.has_value()) {
            parser.SetCursor(parserCursor);
            return std::nullopt;
        }

        std::string string{ std::get<std::string>(stringLiteral->_value) };

        AstStringLiteralExpression stringLiteralNode{parsing::AstStringLiteralExpression(std::move(string)) };
        stringLiteralNode._span = stringLiteral->_span;

        return stringLiteralNode;
    }
} // parsing