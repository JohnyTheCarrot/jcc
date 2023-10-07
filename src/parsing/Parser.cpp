//
// Created by johny on 7/20/23.
//

#include <iostream>
#include "Parser.h"
#include "../reporting.h"
#include "ParserRuleBuilder.h"

Parser::Parser(TokenList &&tokenList, const std::string &fileName, std::istream &inputStream)
        : tokens{std::move(tokenList)}, fileName{fileName}, inputStream{ inputStream } {
    inputStream.seekg(0, std::istream::beg);
}

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
    auto declarations{
        Parser::Expect<ASTSpecifierQualifierList>()
            .FollowedBy<ASTIdentifier>()
            .FollowedByIgnore(TokenType::Assign)
            .FollowedBy<ASTExpression>()
            .FollowedByIgnore(TokenType::Semicolon)
            .Builder<ASTDeclaration>([](std::tuple<ASTSpecifierQualifierList, ASTIdentifier>&& declInfo, ASTExpression &&value) {
                ASTSpecifierQualifierList typeSpecifier{ std::get<0>(declInfo) };
                ASTIdentifier identifier{ std::get<1>(declInfo) };

                return ASTDeclaration{ std::move(typeSpecifier), std::move(identifier), std::move(value) };
            })
    };

    std::optional<ASTDeclaration> decl{ declarations.Match(*this, true) };

    std::cout << decl->ToString(0) << std::endl;
}

void Parser::Error(const Span &span, const std::string &message) {
    ::Error(this->fileName, this->inputStream, span, message);
}
