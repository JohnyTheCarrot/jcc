//
// Created by johny on 7/21/23.
//

#include "ASTNode.h"
#include "Parser.h"
#include "ParserRuleBuilder.h"
#include <variant>

ParseResult ASTIdentifier::Parse(Parser &parser) {
    Span identifierSpan;
    TokenMatch identifierMatch;

    parser.Expect(TokenType::Identifier)
            .Match(identifierMatch, identifierSpan);

    if (!identifierMatch)
        return ParseResultError(identifierSpan, "Expected identifier");

    Token token{std::get<Token>(identifierMatch.value)};
    this->identifier = std::get<std::string>(token.value);

    return std::nullopt;
}

ParseResult ASTFunctionSpecifier::Parse(Parser &parser) {
    Span fnSpecifierSpan;
    TokenMatch specifierMatch;

    parser.Expect(TokenType::KeywordInline)
            .Or(TokenType::KeywordNoreturn)
            .Match(specifierMatch, fnSpecifierSpan);

    if (!specifierMatch)
        return ParseResultError(fnSpecifierSpan, "Invalid specifier");

    if (std::get<Token>(specifierMatch.value).type == TokenType::KeywordInline)
        this->specifier = Specifier::Inline;
    else
        this->specifier = Specifier::NoReturn;

    return std::nullopt;
}

ParseResult ASTTypeQualifier::Parse(Parser &parser) {
    Span qualifierTokenSpan;
    TokenMatch qualifierMatch;

    parser.Expect(TokenType::KeywordConst)
            .Or(TokenType::KeywordRestrict)
            .Or(TokenType::KeywordVolatile)
            .Or(TokenType::KeywordAtomic)
            .Match(qualifierMatch, qualifierTokenSpan);

    if (!qualifierMatch)
        return ParseResultError(qualifierTokenSpan, "Invalid qualifier");

    switch (std::get<Token>(qualifierMatch.value).type) {
        case TokenType::KeywordConst:
            this->qualifier = TypeQualifier::Const;
            break;
        case TokenType::KeywordRestrict:
            this->qualifier = TypeQualifier::Restrict;
            break;
        case TokenType::KeywordVolatile:
            this->qualifier = TypeQualifier::Volatile;
            break;
        case TokenType::KeywordAtomic:
            this->qualifier = TypeQualifier::Atomic;
            break;
        default:
            assert(false);
    }

    return std::nullopt;
}

ParseResult ASTTypeSpecifier::Parse(Parser &parser) {
    Span specifierTokenSpan;
    TokenMatch specifierMatch;

    // todo:
    // atomic-type-specifier
    // struct-or-union-specifier
    // enum-specifier
    // typedef-name
    parser.Expect(TokenType::KeywordVoid)
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
            .Match(specifierMatch, specifierTokenSpan);

    if (!specifierMatch)
        return ParseResultError(specifierTokenSpan, "Invalid type specifier");

    switch(std::get<Token>(specifierMatch.value).type) {
        case TokenType::KeywordVoid:
            this->specifier = TypeSpecifier::Void;
            break;
        case TokenType::KeywordChar:
            this->specifier = TypeSpecifier::Char;
            break;
        case TokenType::KeywordShort:
            this->specifier = TypeSpecifier::Short;
            break;
        case TokenType::KeywordInt:
            this->specifier = TypeSpecifier::Int;
            break;
        case TokenType::KeywordLong:
            this->specifier = TypeSpecifier::Long;
            break;
        case TokenType::KeywordFloat:
            this->specifier = TypeSpecifier::Float;
            break;
        case TokenType::KeywordDouble:
            this->specifier = TypeSpecifier::Double;
            break;
        case TokenType::KeywordSigned:
            this->specifier = TypeSpecifier::Signed;
            break;
        case TokenType::KeywordUnsigned:
            this->specifier = TypeSpecifier::Unsigned;
            break;
        case TokenType::KeywordBool:
            this->specifier = TypeSpecifier::Bool;
            break;
        case TokenType::KeywordComplex:
            this->specifier = TypeSpecifier::Complex;
            break;
        default:
            assert(false);
    }

    return std::nullopt;
}

ParseResult ASTStorageClassSpecifier::Parse(Parser &parser) {
    return {};
}

ParseResult ASTDeclarationSpecifiers::Parse(Parser &parser) {
    Span specifierTokenSpan;
    TokenMatch specifierMatch;

    parser.Expect<ASTTypeSpecifier>()
            .Or<ASTTypeQualifier>()
            .Or<ASTFunctionSpecifier>()
            .Match(specifierMatch, specifierTokenSpan);

    if (!specifierMatch) {
        return ParseResultError(specifierTokenSpan, "Invalid declaration specifier");
    }

    return std::nullopt;
}

ParseResult ASTDeclarator::Parse(Parser &parser) {
    Span identifierSpan;
    TokenMatch identifierMatch;

    parser.Expect<ASTIdentifier>()
            .Match(identifierMatch, identifierSpan);

    if (!identifierMatch) {
        return ParseResultError(identifierSpan, "Invalid declarator");
    }

    return std::nullopt;
}

ParseResult ASTInitDeclarator::Parse(Parser &parser) {
    Span declaratorSpan, postfixExpressionSpan;
    TokenMatch declaratorMatch, postfixExpressionMatch;

    parser.Expect<ASTDeclarator>()
            .Match(declaratorMatch, declaratorSpan)
            .FollowedBy(TokenType::Assign)
            .FollowedBy<ASTPostfixExpression>()
            .Match(postfixExpressionMatch, postfixExpressionSpan);

    if (!declaratorMatch) {
        return ParseResultError(declaratorSpan, "Invalid declarator");
    }

    return std::nullopt;
}

ParseResult ASTDeclaratorList::Parse(Parser &parser) {
    Span declaratorSpan;
    TokenMatch declaratorMatch;

    parser.Expect<ASTInitDeclarator>()
            .Match(declaratorMatch, declaratorSpan);

    if (!declaratorMatch) {
        return ParseResultError(declaratorSpan, "Invalid declarator");
    }

    do {
        parser.Expect(TokenType::Comma)
            .FollowedBy<ASTInitDeclarator>()
            .Match(declaratorMatch, declaratorSpan);
    } while(declaratorMatch);

    return std::nullopt;
}

ParseResult ASTDeclaration::Parse(Parser &parser) {
    Span declListSpan;
    TokenMatch declListMatch;

    Span specifierTokenSpan;
    TokenMatch specifierMatch;

    Span semiTokenSpan;
    bool hasSemi;

    parser.Expect<ASTDeclarationSpecifiers>()
            .Match(specifierMatch, specifierTokenSpan)
            .FollowedBy<ASTDeclaratorList>()
            .Match(declListMatch, declListSpan)
            .FollowedBy(TokenType::Semicolon)
            .Match(hasSemi, semiTokenSpan);

    if (!specifierMatch) {
        return std::get<ParseResultError>(specifierMatch.value);
    }

    if (!hasSemi) {
        return ParseResultError(semiTokenSpan, "Expected semicolon");
    }

    return std::nullopt;
}

std::unique_ptr<ASTNode> ASTNode::Clone() const {
    if (const ASTIdentifier *astIdentifier {dynamic_cast<const ASTIdentifier *>(this)})
        return std::make_unique<ASTIdentifier>(*astIdentifier);

    if (const ASTFunctionSpecifier *astFunctionSpecifier {dynamic_cast<const ASTFunctionSpecifier *>(this)})
        return std::make_unique<ASTFunctionSpecifier>(*astFunctionSpecifier);

    if (const ASTTypeQualifier *astTypeQualifier {dynamic_cast<const ASTTypeQualifier *>(this)})
        return std::make_unique<ASTTypeQualifier>(*astTypeQualifier);

    if (const ASTTypeSpecifier *astTypeSpecifier {dynamic_cast<const ASTTypeSpecifier *>(this)})
        return std::make_unique<ASTTypeSpecifier>(*astTypeSpecifier);

    if (const ASTStorageClassSpecifier *astStorageClassSpecifier {dynamic_cast<const ASTStorageClassSpecifier *>(this)})
        return std::make_unique<ASTStorageClassSpecifier>(*astStorageClassSpecifier);

    if (const ASTDeclarationSpecifiers *astDeclarationSpecifiers {dynamic_cast<const ASTDeclarationSpecifiers *>(this)})
        return std::make_unique<ASTDeclarationSpecifiers>(*astDeclarationSpecifiers);

    if (const ASTPrimaryExpression *astPrimaryExpression {dynamic_cast<const ASTPrimaryExpression *>(this)})
        return std::make_unique<ASTPrimaryExpression>(*astPrimaryExpression);

    if (const ASTDeclarator *astDeclarator {dynamic_cast<const ASTDeclarator *>(this)})
        return std::make_unique<ASTDeclarator>(*astDeclarator);

    if (const ASTInitDeclarator *astInitDeclarator {dynamic_cast<const ASTInitDeclarator *>(this)})
        return std::make_unique<ASTInitDeclarator>(*astInitDeclarator);

    if (const ASTDeclaratorList *astDeclaratorList {dynamic_cast<const ASTDeclaratorList *>(this)})
        return std::make_unique<ASTDeclaratorList>(*astDeclaratorList);

    if (const ASTPostfixExpression *astPostfixExpression {dynamic_cast<const ASTPostfixExpression *>(this)})
        return std::make_unique<ASTPostfixExpression>(*astPostfixExpression);

    assert(false);
}

ParseResult ASTPrimaryExpression::Parse(Parser &parser) {
    Span primaryExpressionSpan;
    TokenMatch primaryExpressionMatch;

    parser.Expect<ASTIdentifier>()
            .Or(TokenType::IntegerLiteral)
            .Or(TokenType::StringLiteral)
            // todo: parenthesized expression, generic selection
            .Match(primaryExpressionMatch, primaryExpressionSpan);

    if (!primaryExpressionMatch)
        return ParseResultError(primaryExpressionSpan, "Invalid primary expression");

    return std::nullopt;
}

ParseResult ASTPostfixExpression::Parse(Parser &parser) {
    Span primaryExpressionSpan, postfixExpressionSpan;
    TokenMatch primaryExpressionMatch, postfixExpressionMatch;

    parser.Expect<ASTPrimaryExpression>()
            .Match(primaryExpressionMatch, primaryExpressionSpan);

    if (!primaryExpressionMatch)
        return ParseResultError(primaryExpressionSpan, "Invalid postfix expression, expected it to start with a primary expression");

    do {
        parser.Expect(std::move(
            parser.Expect(TokenType::LeftBracket)
                    .FollowedBy<ASTPrimaryExpression>()
                    .FollowedBy(TokenType::RightBracket)
        ))
            .Or(std::move(
                    parser.Expect(TokenType::Arrow).FollowedBy<ASTIdentifier>()
            ))
            .Or(std::move(
                    parser.Expect(TokenType::Dot).FollowedBy<ASTIdentifier>()
            ))
            .Or(TokenType::Increment)
            .Or(TokenType::Decrement)
            .Match(postfixExpressionMatch, postfixExpressionSpan);
    } while (postfixExpressionMatch);

    return std::nullopt;
}

ParseResult ASTExpression::Parse(Parser &parser) {
    return ParseResult();
}
