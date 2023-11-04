//
// Created by johny on 10/18/23.
//

#include "AstIdentifierExpression.h"
#include "../../Parser.h"

std::optional<parsing::AstIdentifierExpression> parsing::AstIdentifierExpression::Parse(Parser &parser) {
    int parserCursor{ parser.GetCursor() };

    std::optional<Token> ident{ parser.ConsumeIfTokenIs(TokenType::Identifier) };

    if (!ident.has_value()) {
        parser.SetCursor(parserCursor);
        return std::nullopt;
    }

    std::string identString{ std::get<std::string>(ident->_value) };

    AstIdentifierExpression identifier{parsing::AstIdentifierExpression(std::move(identString)) };
    identifier._span = ident->_span;

    return identifier;
}

std::string parsing::AstIdentifierExpression::ToString(size_t depth) const {
    return "AstIdentifierExpression(\"" + _ident + "\")";
}
