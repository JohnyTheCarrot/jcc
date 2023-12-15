//
// Created by johny on 11/4/23.
//

#include "AstConditionalExpression.h"
#include "../../Parser.h"
#include "AstExpression.h"
#include "AstLogicalOrExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstConditionalExpression::Parse(Parser &parser) {
		AstNode::Ptr condition{AstLogicalOrExpression::Parse(parser)};

		if (condition == nullptr)
			return nullptr;

		if (!parser)
			return condition;

		std::optional<Token> questionMark{parser.ConsumeIfTokenIs(TokenType::QuestionMark)};

		if (!questionMark.has_value())
			return condition;

		AstNode::Ptr trueExpression{AstExpression::Parse(parser)};

		if (trueExpression == nullptr || !parser) {
			parser.Error(questionMark->_span, "Expected expression for true branch");
		}

		std::optional<Token> colon{parser.ConsumeIfTokenIs(TokenType::Colon)};

		AstNode::Ptr falseExpression{AstConditionalExpression::Parse(parser)};

		if (falseExpression == nullptr) {
			parser.Error(colon->_span, "Expected expression for false branch");
		}

		return std::make_unique<AstConditionalExpression>(
				std::move(condition), std::move(trueExpression), std::move(falseExpression)
		);
	}

	std::string AstConditionalExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstConditionalExpression, depth,
				{TOSTRING_FIELD_NODE("_condition", *_condition) TOSTRING_FIELD_NODE("trueExpression", *_trueExpression)
						 TOSTRING_FIELD_NODE("falseExpression", *_falseExpression)}
		)
	}
}// namespace parsing
