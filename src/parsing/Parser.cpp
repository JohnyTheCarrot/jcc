//
// Created by johny on 7/20/23.
//

#include <iostream>
#include "Parser.h"
#include "../reporting.h"

Parser::Parser(TokenList &&tokenList, const std::string &fileName, std::istream &inputStream)
        : tokens{std::move(tokenList)}, fileName{fileName}, inputStream{ inputStream } {
    inputStream.seekg(0, std::istream::beg);
}

//ASTTranslationUnit Parser::Parse() {
//    return ASTTranslationUnit();
//}

std::optional<Token> Parser::PeekNextToken() {
    if (this->cursor + 1 >= this->tokens.size())
        return std::nullopt;

    return *this->tokens[this->cursor + 1];
}

void Parser::AdvanceCursor() {
    ++this->cursor;
}

std::optional<Token> Parser::ConsumeIfTokenIs(TokenType tokenType) {
    std::optional<Token> token{this->PeekNextToken()};

    if (!token.has_value())
        return std::nullopt;

    if (token.value().type == tokenType) {
        ++this->cursor;

        return *this->tokens[this->cursor];
    }

    return std::nullopt;
}

void Parser::Parse() {
    ASTTypeQualifier typeQualifier{};
    bool didMatch{ typeQualifier.Parse(*this) };

    std::cout << "Parse knownResult was: " << didMatch << std::endl;
}

ParserRuleBuilder Parser::Expect(TokenType tokenType) {
    return ParserRuleBuilder{*this, tokenType};
}

void Parser::Error(const Span &span, const std::string &message) {
    ::Error(this->fileName, this->inputStream, span, message);
}
