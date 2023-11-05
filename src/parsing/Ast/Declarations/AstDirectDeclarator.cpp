//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstDirectDeclarator.h"
#include "../../Parser.h"
#include "../Expressions/AstIdentifierExpression.h"
#include "AstDeclarator.h"
#include "../Expressions/AstAssignmentExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstDirectDeclarator::Parse(Parser &parser) {
        std::unique_ptr<AstNode> result{};

        std::optional<Token> leftParenDecl{parser.ConsumeIfTokenIs(TokenType::LeftParenthesis) };
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

                if (staticKeyword) {
                    // type qualifier list is optional
                    // assignment expression is mandatory
                    directDeclarator._isStatic = true;

                    std::optional<AstTypeQualifierList> typeQualifierList{ AstTypeQualifierList::Parse(parser) };
                    directDeclarator._typeQualifierList = typeQualifierList;

                    std::unique_ptr<AstNode> assignmentExpression{ AstAssignmentExpression::Parse(parser) };
                    if (!assignmentExpression)
                        parser.Error(staticKeyword->_span, "Expected assignment expression");

                    directDeclarator._assignmentExpression = std::move(assignmentExpression);
                    directDeclarator._lhs = std::move(result);
                    parser.ExpectToken(TokenType::RightBracket);
                    result = std::make_unique<AstDirectDeclarator>(std::move(directDeclarator));
                    continue;
                }

            }

            std::optional<Token> leftParenFunc{ parser.ConsumeIfTokenIs(TokenType::LeftParenthesis) };
            if (leftParenFunc) {
                TODO()
            }

            return result;
        }
    }

    std::string AstDirectDeclarator::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstDirectDeclarator {" << std::endl;

        if (_lhs)
            ss << tabsChildren << "lhs: " << _lhs->ToString(depth + 1) << std::endl;

        ss << tabsChildren << "isStatic: " << (_isStatic ? "true" : "false") << std::endl;

        if (_typeQualifierList)
            ss << tabsChildren << "typeQualifierList: " << _typeQualifierList->ToString(depth + 1) << std::endl;

        if (_assignmentExpression)
            ss << tabsChildren << "assignmentExpression: " << _assignmentExpression->ToString(depth + 1) << std::endl;

        ss << tabs << '}';

        return ss.str();
    }
} // parsing