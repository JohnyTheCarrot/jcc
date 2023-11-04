//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstStaticAssertDeclaration.h"
#include "../../Parser.h"
#include "../Expressions/AstConstantExpression.h"

namespace parsing {
    std::optional<AstStaticAssertDeclaration> AstStaticAssertDeclaration::Parse(Parser &parser) {
        std::optional<Token> keyword{ parser.ConsumeIfTokenIs(TokenType::KeywordStaticAssert) };

        if (!keyword)
            return std::nullopt;

        parser.ExpectToken(TokenType::LeftParenthesis);

        std::unique_ptr<AstNode> constantExpression{ AstConstantExpression::Parse(parser) };

        if (!constantExpression) {
            parser.Error(keyword->_span, "Expected constant expression");
        }

        parser.ExpectToken(TokenType::Comma);

        std::optional<AstStringLiteralExpression> stringLiteral{ AstStringLiteralExpression::Parse(parser) };

        if (!stringLiteral) {
            parser.Error(keyword->_span, "Expected string literal");
        }

        parser.ExpectToken(TokenType::RightParenthesis);
        parser.ExpectToken(TokenType::Semicolon);

        return AstStaticAssertDeclaration{ std::move(constantExpression), *stringLiteral };
    }

    std::string AstStaticAssertDeclaration::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstStaticAssertDeclaration {" << std::endl;
        ss << tabsChildren << "constantExpression: " << _constantExpression->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "message: " << _message.ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing