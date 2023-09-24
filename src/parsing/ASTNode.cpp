//
// Created by johny on 7/21/23.
//

#include "ASTNode.h"
#include "Parser.h"
#include "ParserRuleBuilder.h"
#include <variant>

//std::unique_ptr<ASTNode> ASTNode::Clone() const {
//    if (const ASTIdentifier *astIdentifier {dynamic_cast<const ASTIdentifier *>(this)})
//        return std::make_unique<ASTIdentifier>(*astIdentifier);
//
//    if (const ASTFunctionSpecifier *astFunctionSpecifier {dynamic_cast<const ASTFunctionSpecifier *>(this)})
//        return std::make_unique<ASTFunctionSpecifier>(*astFunctionSpecifier);
//
//    if (const ASTTypeQualifier *astTypeQualifier {dynamic_cast<const ASTTypeQualifier *>(this)})
//        return std::make_unique<ASTTypeQualifier>(*astTypeQualifier);
//
//    if (const ASTTypeSpecifier *astTypeSpecifier {dynamic_cast<const ASTTypeSpecifier *>(this)})
//        return std::make_unique<ASTTypeSpecifier>(*astTypeSpecifier);
//
//    if (const ASTStorageClassSpecifier *astStorageClassSpecifier {dynamic_cast<const ASTStorageClassSpecifier *>(this)})
//        return std::make_unique<ASTStorageClassSpecifier>(*astStorageClassSpecifier);
//
//    if (const ASTDeclarationSpecifiers *astDeclarationSpecifiers {dynamic_cast<const ASTDeclarationSpecifiers *>(this)})
//        return std::make_unique<ASTDeclarationSpecifiers>(*astDeclarationSpecifiers);
//
//    if (const ASTPrimaryExpression *astPrimaryExpression {dynamic_cast<const ASTPrimaryExpression *>(this)})
//        return std::make_unique<ASTPrimaryExpression>(*astPrimaryExpression);
//
//    if (const ASTDeclarator *astDeclarator {dynamic_cast<const ASTDeclarator *>(this)})
//        return std::make_unique<ASTDeclarator>(*astDeclarator);
//
//    if (const ASTInitDeclarator *astInitDeclarator {dynamic_cast<const ASTInitDeclarator *>(this)})
//        return std::make_unique<ASTInitDeclarator>(*astInitDeclarator);
//
//    if (const ASTDeclaratorList *astDeclaratorList {dynamic_cast<const ASTDeclaratorList *>(this)})
//        return std::make_unique<ASTDeclaratorList>(*astDeclaratorList);
//
//    if (const ASTPostfixExpression *astPostfixExpression {dynamic_cast<const ASTPostfixExpression *>(this)})
//        return std::make_unique<ASTPostfixExpression>(*astPostfixExpression);
//
//    assert(false);
//}

//ParseResult ASTPostfixExpression::Parse(Parser &parseFunction) {
//    Span primaryExpressionSpan, postfixExpressionSpan;
//    TokenMatch primaryExpressionMatch, postfixExpressionMatch;
//
//    parseFunction.Expect<ASTPrimaryExpression>()
//            .Build(primaryExpressionMatch, primaryExpressionSpan);
//
//    if (!primaryExpressionMatch)
//        return ParseResultError(primaryExpressionSpan, "Invalid postfix expression, expected it to start with a primary expression");
//
//    do {
//        parseFunction.Expect(std::move(
//            parseFunction.Expect(TokenType::LeftBracket)
//                    .FollowedBy<ASTPrimaryExpression>()
//                    .FollowedBy(TokenType::RightBracket)
//        ))
//            .Or(std::move(
//                    parseFunction.Expect(TokenType::Arrow).FollowedBy<ASTIdentifier>()
//            ))
//            .Or(std::move(
//                    parseFunction.Expect(TokenType::Dot).FollowedBy<ASTIdentifier>()
//            ))
//            .Or(TokenType::Increment)
//            .Or(TokenType::Decrement)
//            .Build(postfixExpressionMatch, postfixExpressionSpan);
//    } while (postfixExpressionMatch);
//
//    return std::nullopt;
//}

