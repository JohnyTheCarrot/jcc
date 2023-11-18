//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstDirectDeclarator.h"
#include "../../Parser.h"
#include "../Expressions/AstIdentifierExpression.h"
#include "AstDeclarator.h"
#include "../Expressions/AstAssignmentExpression.h"
#include "AstParameterTypeList.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    AstNode::Ptr AstDirectDeclarator::Parse(Parser &parser) {
        AstNode::Ptr result{};

        std::optional<Token> leftParenDecl{ parser.ConsumeIfTokenIs(TokenType::LeftParenthesis) };
        if (leftParenDecl) {
            std::optional<AstDeclarator> decl{ AstDeclarator::Parse(parser) };
            if (!decl)
                parser.Error(leftParenDecl->_span, "Expected declarator after left parenthesis");

            parser.ExpectToken(TokenType::RightParenthesis);

            result = std::make_unique<AstDeclarator>(std::move(*decl));
        } else {
            std::optional<AstIdentifierExpression> identifier{AstIdentifierExpression::Parse(parser)};

            if (identifier) {
                result = std::make_unique<AstIdentifierExpression>(std::move(identifier.value()));
            } else {
                return nullptr;
            }
        }

        while (true) {
            AstDirectDeclarator directDeclarator;

            std::optional<Token> leftBracket{ parser.ConsumeIfTokenIs(TokenType::LeftBracket) };
            if (leftBracket) {
                std::optional<Token> staticKeyword{ parser.ConsumeIfTokenIs(TokenType::KeywordStatic) };
                directDeclarator._kind = Kind::Array;

                if (staticKeyword) {
                    // type qualifier list is optional
                    // assignment expression is mandatory
                    directDeclarator._isStatic = true;

                    std::optional<AstTypeQualifierList> typeQualifierList{ AstTypeQualifierList::Parse(parser) };
                    directDeclarator._typeQualifierList = typeQualifierList;

                    AstNode::Ptr assignmentExpression{ AstAssignmentExpression::Parse(parser) };
                    if (!assignmentExpression)
                        parser.Error(staticKeyword->_span, "Expected assignment expression");

                    directDeclarator._assignmentExpression = std::move(assignmentExpression);
                    directDeclarator._lhs = std::move(result);
                    parser.ExpectToken(TokenType::RightBracket);
                    result = std::make_unique<AstDirectDeclarator>(std::move(directDeclarator));
                    continue;
                }

                std::optional<AstTypeQualifierList> typeQualifierList{ AstTypeQualifierList::Parse(parser) };

                std::optional<Token> staticToken{parser.ConsumeIfTokenIs(TokenType::KeywordStatic) };

                if (staticToken) {
                    directDeclarator._isStatic = true;
                    directDeclarator._lhs = std::move(result);
                    directDeclarator._typeQualifierList = typeQualifierList;

                    AstNode::Ptr assignmentExpression{ AstAssignmentExpression::Parse(parser) };
                    if (!assignmentExpression)
                        parser.Error(staticKeyword->_span, "Expected assignment expression");

                    directDeclarator._assignmentExpression = std::move(assignmentExpression);
                    parser.ExpectToken(TokenType::RightBracket);
                    result = std::make_unique<AstDirectDeclarator>(std::move(directDeclarator));
                    continue;
                }

                std::optional<Token> asterisk{ parser.ConsumeIfTokenIs(TokenType::Asterisk) };
                if (asterisk)
                {
                    directDeclarator._isVLA = true;
                    directDeclarator._typeQualifierList = typeQualifierList;
                    directDeclarator._lhs = std::move(result);

                    result = std::make_unique<AstDirectDeclarator>(std::move(directDeclarator));
                    parser.ExpectToken(TokenType::RightBracket);
                    continue;
                }

                AstNode::Ptr assignmentExpression{ AstAssignmentExpression::Parse(parser) };
                directDeclarator._typeQualifierList = typeQualifierList;
                directDeclarator._lhs = std::move(result);

                if (assignmentExpression) {
                    directDeclarator._assignmentExpression = std::move(assignmentExpression);
                }

                result = std::make_unique<AstDirectDeclarator>(std::move(directDeclarator));
                parser.ExpectToken(TokenType::RightBracket);

                return result;
            }

            std::optional<Token> leftParenFunc{ parser.ConsumeIfTokenIs(TokenType::LeftParenthesis) };
            if (leftParenFunc) {
                AstNode::Ptr parameterTypeList{ AstParameterTypeList::Parse(parser) };
                directDeclarator._kind = Kind::Function;
                directDeclarator._lhs = std::move(result);
                parser.ExpectToken(TokenType::RightParenthesis);

                if (parameterTypeList)
                    directDeclarator._parameterTypeList = std::move(parameterTypeList);

                result = std::make_unique<AstDirectDeclarator>(std::move(directDeclarator));
                return result;
            }

            return result;
        }
    }

    std::string AstDirectDeclarator::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstDirectDeclarator, depth, {
            if (_lhs)
                TOSTRING_FIELD_NODE("lhs", *_lhs)

            TOSTRING_FIELD_ENUM("kind", _kind)
            TOSTRING_FIELD_BOOL("isStatic", _isStatic)
            TOSTRING_FIELD_BOOL("isVLA", _isVLA)

            if (_typeQualifierList)
                TOSTRING_FIELD_NODE("typeQualifierList", *_typeQualifierList)

            if (_assignmentExpression)
                TOSTRING_FIELD_NODE("assignmentExpression", *_assignmentExpression)

            if (_parameterTypeList)
                TOSTRING_FIELD_NODE("parameterTypeList", *_parameterTypeList)
        })
    }
} // parsing