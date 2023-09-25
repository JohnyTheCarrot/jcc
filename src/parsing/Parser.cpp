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
    auto identifiers = Parser::Expect(TokenType::Identifier)
            .Builder<ASTIdentifier>([](Token token) {
                return ASTIdentifier{ std::get<std::string>(token.value) };
            });

    auto constant = Parser::Expect(TokenType::StringLiteral)
            .Or(TokenType::IntegerLiteral)
            .Or(TokenType::FloatLiteral)
            .Or(TokenType::DoubleLiteral)
            .Builder<ASTConstant>([](const Token& token) {
                return ASTConstant{ token };
            });

    auto primaryExpression = Parser::Expect<typeof identifiers, ASTPrimaryExpression, ASTIdentifier>(identifiers)
            .Or(constant)
            .Builder<ASTPrimaryExpression>([](const std::variant<ASTIdentifier, ASTConstant> &variant) {
                return ASTPrimaryExpression{ variant };
            });

    auto postfixIncrementExpression{
            Parser::Expect(TokenType::Increment)
                    .Builder<ASTPostfixIncrement>([](const Token &token) {
                        return ASTPostfixIncrement{};
                    })
    };

    auto postfixDecrementExpression{
            Parser::Expect(TokenType::Decrement)
                    .Builder<ASTPostfixDecrement>([](const Token &token) {
                        return ASTPostfixDecrement{};
                    })
    };

    auto postfixArrowExpression{
            Parser::Expect(TokenType::Arrow)
                    .FollowedBy(identifiers)
                    .Builder<ASTPostfixArrow>([](const Token &, const ASTIdentifier &identifier) {
                        return ASTPostfixArrow{ identifier };
                    })
    };

    auto postfixDotExpression{
            Parser::Expect(TokenType::Dot)
                    .FollowedBy(identifiers)
                    .Builder<ASTPostfixDot>([](const Token &, const ASTIdentifier &identifier) {
                        return ASTPostfixDot{ identifier };
                    })
    };

    auto postfixExpression = Parser::Expect<typeof primaryExpression, ASTPostfixExpression, ASTPrimaryExpression>(primaryExpression)
            .FollowedBy(
                    Parser::Expect<typeof postfixIncrementExpression, ASTPostfixIncrement, Token>(postfixIncrementExpression)
                            .Or(postfixDecrementExpression)
                            .Or(postfixArrowExpression)
                            .Or(postfixDotExpression)
                            .ZeroOrMore()
            ).Builder<ASTPostfixExpression>(
                    [](
                            const ASTPrimaryExpression &primaryExpressionNode,
                            const std::vector<typename ASTPostfixExpression::Operation> &postfixExpressions
                    ) {
                ASTPostfixExpression output{primaryExpressionNode, std::nullopt};

                for (const typename ASTPostfixExpression::Operation &item: postfixExpressions) {
                    output = ASTPostfixExpression{std::move(output), item};
                }

                return output;
            });

    auto typeSpecifier = Parser::Expect(TokenType::KeywordVoid)
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

    auto declarations{
        Parser::Expect<typeof typeSpecifier, ASTDeclaration, ASTTypeSpecifier>(typeSpecifier)
            .FollowedBy(identifiers)
            .FollowedByIgnore(TokenType::Assign)
            .FollowedBy(postfixExpression)
            .FollowedByIgnore(TokenType::Semicolon)
            .Builder<ASTDeclaration>([](const std::tuple<ASTTypeSpecifier, ASTIdentifier>& declInfo, const ASTPostfixExpression &value) {
                ASTTypeSpecifier typeSpecifier{ std::get<0>(declInfo) };
                ASTIdentifier identifier{ std::get<1>(declInfo) };

                return ASTDeclaration{ typeSpecifier, identifier, value };
            })
    };

    std::optional<ASTDeclaration> decl{ declarations.Match(*this, true) };

    std::cout << decl->ToString(0) << std::endl;
}

void Parser::Error(const Span &span, const std::string &message) {
    ::Error(this->fileName, this->inputStream, span, message);
}
