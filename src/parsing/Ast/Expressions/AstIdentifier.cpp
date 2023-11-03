//
// Created by johny on 10/18/23.
//

#include "AstIdentifier.h"
#include "../../Parser.h"

std::optional<parsing::AstIdentifier> parsing::AstIdentifier::Parse(Parser &parser) {
    int parserCursor{ parser.GetCursor() };

    std::optional<Token> ident{ parser.ConsumeIfTokenIs(TokenType::Identifier) };

    if (!ident.has_value()) {
        parser.SetCursor(parserCursor);
        return std::nullopt;
    }

    std::string identString{ std::get<std::string>(ident->_value) };

    AstIdentifier identifier{ parsing::AstIdentifier(std::move(identString)) };
    identifier._span = ident->_span;

    return identifier;
}

std::string parsing::AstIdentifier::ToString(size_t depth) const {
    return "AstIdentifier(\"" + _ident + "\")";
}
