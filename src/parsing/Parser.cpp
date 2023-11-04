//
// Created by johny on 7/20/23.
//

#include <iostream>
#include <utility>
#include <sstream>
#include "Parser.h"
#include "../reporting.h"
#include "Ast/Expressions/AstIdentifierExpression.h"
#include "Ast/Expressions/AstExpression.h"
#include "../../libs/magic_enum/magic_enum.hpp"
#include "Ast/Declarations/AstDeclaration.h"
#include "Ast/Declarations/AstDeclarationSpecifiers.h"

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
    std::optional<AstDeclarationSpecifiers> astDeclaration{ AstDeclarationSpecifiers::Parse(*this) };
    if (astDeclaration)
        std::cout << astDeclaration->ToString(0) << std::endl;
    else
        std::cout << "didn't match" << std::endl;
//    std::unique_ptr<AstNode> identifier{ AstExpression::Parse(*this) };
//    if (identifier != nullptr)
//        std::cout << identifier->ToString(0) << std::endl;
//    else
//        std::cout << "didn't match" << std::endl;
}

void Parser::Error(const Span &span, const std::string &message) const {
    ::Error(this->fileName, this->inputStream, span, message);
}

bool Parser::operator!() const {
    return this->cursor + 1 == this->tokens.size();
}

Parser::operator bool() const {
    return !!*this;
}

bool Parser::AdvanceIfTokenIs(TokenType tokenValue) {
    if (!*this)
        return false;

    bool isTokenMatch{this->PeekNextToken()._type == tokenValue };

    if (isTokenMatch)
        this->AdvanceCursor();

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
