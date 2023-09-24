//
// Created by johny on 7/21/23.
//

#include "ParserRuleBuilder.h"
#include "../reporting.h"
#include "Parser.h"

ParserRuleToken &ParserRuleToken::Or(TokenType tokenType) {
    this->alternatives.push_back(tokenType);

    return *this;
}

void Error(Parser &parser, const std::string &message) {
    parser.Error(parser.GetLastToken()->span, message);
}

bool IsNextTokenMatch(Parser &parser, TokenType type) {
    std::optional<Token> token{ parser.ConsumeIfTokenIs(type) };

    return token.has_value();
}

std::optional<ParserRuleToken::OutNode> ParserRuleToken::Match(Parser &parser) const {
    std::optional<Token> potentialToken{parser.PeekNextToken()};

    if (!potentialToken.has_value()) {
        return std::nullopt;
    }

    Token &token = potentialToken.value();

    if (token.type == this->toMatch) {
        parser.AdvanceCursor();
        return token;
    }

    for(TokenType alternative : this->alternatives) {
        if (token.type == alternative) {
            parser.AdvanceCursor();
            return token;
        }
    }

    return std::nullopt;
}
