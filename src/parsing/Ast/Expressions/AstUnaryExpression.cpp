//
// Created by johny on 11/3/23.
//

#include "AstUnaryExpression.h"
#include "../../../tokenizer.h"
#include "../../Parser.h"
#include "AstCastExpression.h"
#include "AstPostfixExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstUnaryExpression::Parse(Parser &parser) {
		if (!parser)
			return nullptr;

		const Token &token{parser.PeekNextToken()};

		switch (token._type) {
			case TokenType::Increment: {
				parser.AdvanceCursor();
				AstNode::Ptr expression{AstUnaryExpression::Parse(parser)};

				if (expression == nullptr) {
					parser.Error(token._span, "Expected expression");
				}

				return std::make_unique<AstUnaryExpression>(AstUnaryOperator::PreIncrement, std::move(expression));
			}
			case TokenType::Decrement: {
				parser.AdvanceCursor();
				AstNode::Ptr expression{AstUnaryExpression::Parse(parser)};

				if (expression == nullptr) {
					parser.Error(token._span, "Expected expression");
				}

				return std::make_unique<AstUnaryExpression>(AstUnaryOperator::PreDecrement, std::move(expression));
			}
			case TokenType::Ampersand:
			case TokenType::Asterisk:
			case TokenType::Plus:
			case TokenType::Minus:
			case TokenType::BitwiseNot:
			case TokenType::LogicalNot: {
				parser.AdvanceCursor();
				AstNode::Ptr expression{AstCastExpression::Parse(parser)};

				if (expression == nullptr) {
					parser.Error(token._span, "Expected expression");
				}

				AstUnaryOperator unaryOperator;

				switch (token._type) {
					case TokenType::Ampersand:
						unaryOperator = AstUnaryOperator::AddressOf;
						break;
					case TokenType::Asterisk:
						unaryOperator = AstUnaryOperator::Dereference;
						break;
					case TokenType::Plus:
						unaryOperator = AstUnaryOperator::Plus;
						break;
					case TokenType::Minus:
						unaryOperator = AstUnaryOperator::Minus;
						break;
					case TokenType::BitwiseNot:
						unaryOperator = AstUnaryOperator::BitwiseNot;
						break;
					case TokenType::LogicalNot:
						unaryOperator = AstUnaryOperator::Not;
						break;
					default:
						__builtin_unreachable();
				}

				return std::make_unique<AstUnaryExpression>(unaryOperator, std::move(expression));
			}
			case TokenType::KeywordSizeof:
			case TokenType::KeywordAlignof:
				TODO()
			default:
				return AstPostfixExpression::Parse(parser);
		}
	}

	std::string AstUnaryExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstUnaryExpression, depth,
				{TOSTRING_FIELD_ENUM("unaryOperator", _unaryOperator) TOSTRING_FIELD_NODE("operand", *_operand)}
		)
	}
}// namespace parsing
