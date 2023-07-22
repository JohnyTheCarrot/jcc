//
// Created by johny on 7/21/23.
//

#include "ASTNode.h"
#include "Parser.h"

bool ASTFunctionSpecifier::Parse(Parser &parser) {
    Span fnSpecifierSpan;
    TokenMatch specifierToken;

    parser.Expect(TokenType::KeywordInline)
            .Or(TokenType::KeywordNoreturn)
            .Match(specifierToken, fnSpecifierSpan);

    if (!specifierToken.has_value())
        return false;

    if (specifierToken.value().type == TokenType::KeywordInline)
        this->specifier = Specifier::Inline;
    else
        this->specifier = Specifier::NoReturn;

    return true;
}

bool ASTTypeQualifier::Parse(Parser &parser) {
    Span specifierTokenSpan;
    Span semiSpan;
    TokenMatch qualifierToken;
    bool hasSemi;

    parser.Expect(TokenType::KeywordConst)
            .Or(TokenType::KeywordRestrict)
            .Or(TokenType::KeywordVolatile)
            .Or(TokenType::KeywordAtomic)
            .Match(qualifierToken, specifierTokenSpan)
            .FollowedBy(TokenType::Semicolon)
            .Match(hasSemi, semiSpan);

    if (!qualifierToken.has_value()) {
        parser.Error(specifierTokenSpan, "Invalid specifier");
    }

    if (!hasSemi) {
        parser.Error(semiSpan, "Missing semicolon");
    }

    switch (qualifierToken.value().type) {
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

    return true;
}

bool ASTTypeSpecifier::Parse(Parser &parser) {
    // todo:
    // atomic-type-specifier
    // struct-or-union-specifier
    // enum-specifier
    // typedef-name
//    parser.Expect(TokenType::KeywordVoid)
//            .Or(TokenType::KeywordChar)
//            .Or(TokenType::KeywordShort)
//            .Or(TokenType::KeywordInt)
//            .Or(TokenType::KeywordLong)
//            .Or(TokenType::KeywordFloat)
//            .Or(TokenType::KeywordDouble)
//            .Or(TokenType::KeywordSigned)
//            .Or(TokenType::KeywordUnsigned)
//            .Or(TokenType::KeywordBool)
//            .Or(TokenType::KeywordComplex)
//            .Match(matchedTokens);
}
