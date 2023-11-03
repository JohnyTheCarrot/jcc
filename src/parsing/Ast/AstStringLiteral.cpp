//
// Created by johny on 11/3/23.
//

#include "AstStringLiteral.h"
#include "../Parser.h"

namespace parsing {
    std::string AstStringLiteral::ToString(size_t depth) const {
        return "AstStringLiteral(\"" + _string + "\")";
    }

    std::optional<AstStringLiteral> AstStringLiteral::Parse(Parser &parser) {
        int parserCursor{ parser.GetCursor() };

        std::optional<Token> stringLiteral{ parser.ConsumeIfTokenIs(TokenType::StringLiteral) };

        if (!stringLiteral.has_value()) {
            parser.SetCursor(parserCursor);
            return std::nullopt;
        }

        std::string string{ std::get<std::string>(stringLiteral->value) };

        AstStringLiteral stringLiteralNode{ parsing::AstStringLiteral(std::move(string)) };
        stringLiteralNode._span = stringLiteral->_span;

        return stringLiteralNode;
    }
} // parsing