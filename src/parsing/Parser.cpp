//
// Created by johny on 7/20/23.
//

#include <iostream>
#include <utility>
#include <sstream>
#include "Parser.h"
#include "../reporting.h"
#include "../../libs/magic_enum/magic_enum.hpp"
#include "Ast/ExternalDefinitions/AstTranslationUnit.h"

using namespace parsing;

Parser::Parser(TokenList &&tokenList, std::string fileName, std::istream &inputStream)
        : tokens{std::move(tokenList)}, fileName{std::move(fileName)}, inputStream{ inputStream } {
    inputStream.seekg(0, std::istream::beg);
}

const Token& Parser::PeekNextToken() {
    assert(static_cast<bool>(*this) && "No next token");

    return *this->tokens[this->cursor + 1];
}

void Parser::AdvanceCursor() {
    assert(static_cast<bool>(*this) && "No next token");

    ++this->cursor;
}

std::optional<Token> Parser::ConsumeIfTokenIs(TokenType tokenType) {
    if (!*this)
        return std::nullopt;

    Token token{ this->PeekNextToken() };

    if (token._type == tokenType) {
        this->AdvanceCursor();

        return *this->tokens[this->cursor];
    }

    return std::nullopt;
}

void Parser::Parse() {
    std::optional<AstTranslationUnit> translationUnit{AstTranslationUnit::Parse(*this) };
    if (translationUnit)
        std::cout << translationUnit->ToString(0) << std::endl;
    else
        std::cout << "didn't match" << std::endl;
}

void Parser::Error(const Span &span, const std::string &message) const {
    ::Error(this->fileName, this->inputStream, span, message);
}

void Parser::Error(const std::string &message) const {
    int currentCursor{ this->cursor };

    if (currentCursor == -1)
        currentCursor = 0;

    Span span{ this->tokens[currentCursor]->_span };

    this->Error(span, message);
}

bool Parser::operator!() const {
    return this->cursor + 1 == this->tokens.size();
}

Parser::operator bool() const {
    return !!*this;
}

bool Parser::AdvanceIfTokenIs(TokenType tokenType, Token &token) {
    if (!*this)
        return false;

    const Token &nextToken{ this->PeekNextToken() };

    bool isTokenMatch{nextToken._type == tokenType };

    if (isTokenMatch) {
        this->AdvanceCursor();
        token = nextToken;
    }

    return isTokenMatch;
}

const Token &Parser::ExpectToken(TokenType tokenType) {
    std::stringstream errorMessage{ };
    errorMessage << "Expected " << magic_enum::enum_name(tokenType) << " but got ";

    if (!*this) {
        errorMessage << "end of file";
        Span span{};
        if (this->tokens.size() > 0)
            span = this->tokens[this->tokens.size() - 1]->_span;

        this->Error(span, errorMessage.str());
    }

    const Token &token{ this->PeekNextToken() };

    if (token._type != tokenType) {
        errorMessage << magic_enum::enum_name(token._type);
        this->Error(this->PeekNextToken()._span, errorMessage.str());
    }

    this->AdvanceCursor();

    return token;
}

bool Parser::AdvanceIfTokenIs(TokenType tokenType) {
    Token token;
    return this->AdvanceIfTokenIs(tokenType, token);
}
