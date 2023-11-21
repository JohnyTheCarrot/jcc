//
// Created by johny on 11/18/23.
//

#include "AstArgumentExpressionList.h"
#include "../../Parser.h"
#include "AstAssignmentExpression.h"

namespace parsing {
	std::optional<AstArgumentExpressionList> AstArgumentExpressionList::Parse(Parser &parser) {
		Span span{};
		std::vector<AstNode::Ptr> expressions{};
		AstNode::Ptr assignmentExpression{AstAssignmentExpression::Parse(parser)};
		if (!assignmentExpression) return std::nullopt;

		span += assignmentExpression->_span;
		expressions.push_back(std::move(assignmentExpression));

		while (true) {
			std::optional<Token> comma{parser.ConsumeIfTokenIs(TokenType::Comma)};
			if (!comma) return AstArgumentExpressionList(span, std::move(expressions));

			assignmentExpression = AstAssignmentExpression::Parse(parser);
			if (!assignmentExpression)
				parser.Error(comma->_span, "Expected to be followed by an assignment expression");

			span += assignmentExpression->_span;
			expressions.push_back(std::move(assignmentExpression));
		}
	}

	std::string AstArgumentExpressionList::ToString(size_t depth) const {
		TOSTRING_LIST(AstArgumentExpressionList, depth, {
			for (const auto &expression: _expressions) { TOSTRING_LIST_ITEM_NODE(*expression) }
		})
	}
}// namespace parsing
