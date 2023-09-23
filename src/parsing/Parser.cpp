//
// Created by johny on 7/20/23.
//

#include <iostream>
#include "Parser.h"
#include "../reporting.h"
#include "../../libs/magic_enum/magic_enum.hpp"

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
    auto typeSpecifier = Parser::ExpectNew(TokenType::KeywordVoid)
            .Or(TokenType::KeywordChar)
            .Or(TokenType::KeywordShort)
            .Or(TokenType::KeywordInt)
            .Or(TokenType::KeywordLong)
            .Or(TokenType::KeywordFloat)
            .Or(TokenType::KeywordDouble)
            .Or(TokenType::KeywordSigned)
            .Or(TokenType::KeywordUnsigned)
            .Or(TokenType::KeywordBool)
            .Or(TokenType::KeywordComplex)
            .Builder<ASTTypeSpecifier>([](const Token& token) {
                ASTTypeSpecifier::TypeSpecifier typeSpecifier{ ASTTypeSpecifier::FromTokenType(token.type) };

                return ASTTypeSpecifier{typeSpecifier};
            });

    auto identifiers = Parser::ExpectNew(TokenType::Identifier)
        .Builder<ASTIdentifier>([](Token token) {
            return ASTIdentifier{ std::get<std::string>(token.value) };
        });

    auto constant = Parser::ExpectNew(TokenType::StringLiteral)
            .Or(TokenType::IntegerLiteral)
            .Or(TokenType::FloatLiteral)
            .Or(TokenType::DoubleLiteral)
            .Builder<ASTConstant>([](const Token& token) {
                return ASTConstant{ token };
            });

    auto declarations{
        Parser::ExpectNew<typeof typeSpecifier, ASTDeclaration, ASTTypeSpecifier>(typeSpecifier)
            .FollowedBy(identifiers)
            .FollowedByIgnore(TokenType::Assign)
            .FollowedBy(constant)
            .FollowedByIgnore(TokenType::Semicolon)
            .Builder<ASTDeclaration>([](const std::tuple<ASTTypeSpecifier, ASTIdentifier>& declInfo, const ASTConstant &value) {
                ASTTypeSpecifier typeSpecifier{ std::get<0>(declInfo) };
                ASTIdentifier identifier{ std::get<1>(declInfo) };

                return ASTDeclaration{ typeSpecifier, identifier.identifier, value };
            })
    };

    ASTDeclaration decl{ declarations.Match(*this) };

    std::cout << "Parse success: Specifier("
        << magic_enum::enum_name(decl.specifier.specifier)
        << ") Identifier(" << decl.identifier << ") "
        << "Constant(";

    switch (decl.constantValue.constantToken.type) {
        case TokenType::StringLiteral:
            std::cout << '"' << std::get<std::string>(decl.constantValue.constantToken.value) << '"';
            break;
        case TokenType::IntegerLiteral:
            std::cout << std::to_string(std::get<IntegerLiteralTokenValue>(decl.constantValue.constantToken.value).value);
            break;
        case TokenType::DoubleLiteral:
            std::cout << std::to_string(std::get<double>(decl.constantValue.constantToken.value));
            break;
        default:
            assert(false);
    }

    std::cout << ")"
        << std::endl;
}

ParserRuleBuilder Parser::Expect(ParserRuleBuilder &&ruleBuilder) {
    return {*this, ParserRuleBuilder::GetRule(std::move(ruleBuilder))};
}

ParserRuleBuilder Parser::Expect(TokenType tokenType) {
    return {*this, tokenType};
}

void Parser::Error(const Span &span, const std::string &message) {
    ::Error(this->fileName, this->inputStream, span, message);
}