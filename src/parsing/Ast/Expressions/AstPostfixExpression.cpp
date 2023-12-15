//
// Created by johny on 11/3/23.
//

#include "AstPostfixExpression.h"
#include "../../Parser.h"
#include "AstArgumentExpressionList.h"
#include "AstExpression.h"
#include "AstIdentifierExpression.h"
#include "AstPrimaryExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstPostfixExpression::Parse(Parser &parser) {
		AstNode::Ptr left{AstPrimaryExpression::Parse(parser)};

		if (left == nullptr) {
			return nullptr;
		}

		while (true) {
			int parserCursor{parser.GetCursor()};

			if (!parser)
				return left;

			const Token &nextToken{parser.PeekNextToken()};

			switch (nextToken._type) {
				case TokenType::LeftBracket: {
					parser.AdvanceCursor();

					AstNode::Ptr expression{AstExpression::Parse(parser)};
					std::optional<Token> rightBracket{parser.ConsumeIfTokenIs(TokenType::RightBracket)};

					if (expression == nullptr) {
						parser.Error(nextToken._span, "Expected expression");
					} else if (!rightBracket.has_value()) {
						parser.Error(expression->_span, "Expected to be followed by ']'");
					}

					Span expressionSpan{expression->_span};
					left = std::make_unique<AstPostfixExpression>(
							std::move(left), PostfixExpressionType::ArraySubscript, std::move(expression)
					);
					left->_span = nextToken._span + expressionSpan + rightBracket->_span;
					break;
				}
				case TokenType::Dot: {
					parser.AdvanceCursor();

					std::optional<AstIdentifierExpression> identifier{AstIdentifierExpression::Parse(parser)};

					if (!identifier.has_value()) {
						parser.Error(nextToken._span, "Expected identifier");
					}

					Span identifierSpan{identifier->_span};
					AstNode::Ptr identifierNode{std::make_unique<AstIdentifierExpression>(std::move(identifier.value()))
					};

					left = std::make_unique<AstPostfixExpression>(
							std::move(left), PostfixExpressionType::MemberAccess, std::move(identifierNode)
					);
					left->_span = nextToken._span + identifierSpan;
					break;
				}
				case TokenType::Arrow: {
					parser.AdvanceCursor();

					std::optional<AstIdentifierExpression> identifier{AstIdentifierExpression::Parse(parser)};

					if (!identifier.has_value()) {
						parser.Error(nextToken._span, "Expected identifier");
					}

					Span identifierSpan{identifier->_span};
					AstNode::Ptr identifierNode{std::make_unique<AstIdentifierExpression>(std::move(identifier.value()))
					};

					left = std::make_unique<AstPostfixExpression>(
							std::move(left), PostfixExpressionType::PointerMemberAccess, std::move(identifierNode)
					);
					left->_span = nextToken._span + identifierSpan;
					break;
				}
				case TokenType::Increment: {
					parser.AdvanceCursor();

					left = std::make_unique<AstPostfixExpression>(
							std::move(left), PostfixExpressionType::PostIncrement, nullptr
					);
					left->_span = nextToken._span;
					break;
				}
				case TokenType::Decrement: {
					parser.AdvanceCursor();

					left = std::make_unique<AstPostfixExpression>(
							std::move(left), PostfixExpressionType::PostDecrement, nullptr
					);
					left->_span = nextToken._span;
					break;
				}
				case TokenType::LeftParenthesis: {
					parser.AdvanceCursor();
					std::optional<AstArgumentExpressionList> argumentList{AstArgumentExpressionList::Parse(parser)};
					parser.ExpectToken(TokenType::RightParenthesis);

					left = std::make_unique<AstPostfixExpression>(
							std::move(left), PostfixExpressionType::FunctionCall,
							std::make_unique<AstArgumentExpressionList>(std::move(*argumentList))
					);
					break;
				}
				default:
					parser.SetCursor(parserCursor);
					return left;
			}
		}
	}

	std::string AstPostfixExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstPostfixExpression, depth, {
			TOSTRING_FIELD_NODE("left", *_left)
			TOSTRING_FIELD_ENUM("type", _postfixExpressionType)
			if (_right != nullptr)
				TOSTRING_FIELD_NODE("right", *_right)
		})
	}
}// namespace parsing
