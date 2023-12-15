//
// Created by johny on 11/22/23.
//

#include "AstDirectAbstractDeclarator.h"
#include "../../Parser.h"
#include "../Expressions/AstAssignmentExpression.h"
#include "AstAbstractDeclarator.h"
#include "AstParameterTypeList.h"

namespace parsing {
	AstNode::Ptr AstDirectAbstractDeclarator::Parse(Parser &parser) {
		std::unique_ptr<AstDirectAbstractDeclarator> result{nullptr};

		Span span{};
		while (true) {
			const Token &firstToken{parser.PeekNextToken()};

			switch (firstToken._type) {
				case TokenType::LeftParenthesis: {
					parser.AdvanceCursor();
					span += firstToken._span;
					std::optional<AstAbstractDeclarator> abstractDeclarator{AstAbstractDeclarator::Parse(parser)};
					if (!abstractDeclarator) {
						AstNode::Ptr parameterTypeList{AstParameterTypeList::Parse(parser)};
						parser.ExpectToken(TokenType::RightParenthesis, span);
						result = std::make_unique<AstDirectAbstractDeclarator>(span, Kind::Function, std::move(result));
						result->_parameterTypeList = std::move(parameterTypeList);
						continue;
					}

					parser.ExpectToken(TokenType::RightParenthesis, span);
					result = std::make_unique<AstDirectAbstractDeclarator>(span, Kind::Function, std::move(result));
					result->_abstractDeclarator = std::move(abstractDeclarator);
					continue;
				}
				case TokenType::LeftBracket: {
					parser.AdvanceCursor();
					std::optional<Token> vlaAsteriskToken{parser.ConsumeIfTokenIs(TokenType::Asterisk)};
					if (vlaAsteriskToken) {
						result = std::make_unique<AstDirectAbstractDeclarator>(span, Kind::Array, std::move(result));
						result->_isVLA = true;
						parser.ExpectToken(TokenType::RightBracket, span);
						continue;
					}

					std::optional<AstTypeQualifierList> typeQualifierList{AstTypeQualifierList::Parse(parser)};

					const std::optional<Token> staticToken{parser.ConsumeIfTokenIs(TokenType::KeywordStatic)};
					if (staticToken) {
						span += staticToken->_span;
						if (!typeQualifierList)
							typeQualifierList = AstTypeQualifierList::Parse(parser);
					}

					AstNode::Ptr assignmentExpression{AstAssignmentExpression::Parse(parser)};
					if (!assignmentExpression && staticToken.has_value())
						parser.Error(staticToken->_span, "Expected to be followed by an assignment expression");
					parser.ExpectToken(TokenType::RightParenthesis, span);

					result = std::make_unique<AstDirectAbstractDeclarator>(span, Kind::Array, std::move(result));
					result->_assignmentExpression = std::move(assignmentExpression);
					result->_typeQualifierList = std::move(typeQualifierList);
					result->_isStatic = staticToken.has_value();

					continue;
				}
				default:
					return result;
			}
		}
	}

	std::string AstDirectAbstractDeclarator::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstDirectAbstractDeclarator, depth, {
			TOSTRING_FIELD_ENUM("kind", _kind)
			TOSTRING_FIELD_BOOL("isStatic", _isStatic)
			TOSTRING_FIELD_BOOL("isVLA", _isVLA)
			if (_lhs)
				TOSTRING_FIELD_NODE("lhs", *_lhs)
			if (_parameterTypeList)
				TOSTRING_FIELD_NODE("parameterTypeList", *_parameterTypeList)
			if (_assignmentExpression)
				TOSTRING_FIELD_NODE("assignmentExpression", *_assignmentExpression)
			if (_abstractDeclarator)
				TOSTRING_FIELD_NODE("abstractDeclarator", *_abstractDeclarator)
			if (_typeQualifierList)
				TOSTRING_FIELD_NODE("typeQualifierList", *_typeQualifierList)
		})
	}
}// namespace parsing
