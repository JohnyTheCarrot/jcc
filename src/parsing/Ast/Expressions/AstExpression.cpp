//
// Created by johny on 11/2/23.
//

#include "AstExpression.h"
#include "../../Parser.h"
#include "AstAssignmentExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstExpression::Parse(Parser &parser) {
		AstNode::Ptr left{AstAssignmentExpression::Parse(parser)};

		if (left == nullptr)
			return nullptr;

		std::vector<AstNode::Ptr> expressions{};
		expressions.push_back(nullptr);

		while (true) {
			if (!parser)
				break;

			std::optional<Token> token{parser.ConsumeIfTokenIs(TokenType::Comma)};

			if (!token.has_value())
				break;

			AstNode::Ptr right{AstAssignmentExpression::Parse(parser)};

			if (right == nullptr)
				parser.Error(token->_span, "Expected expression");

			expressions.push_back(std::move(right));
		}

		if (expressions.size() == 1)
			return left;

		expressions[0] = std::move(left);

		return std::make_unique<AstExpression>(std::move(expressions));
	}

	std::string AstExpression::ToString(size_t depth) const {
		TOSTRING_LIST(AstExpression, depth, {
			for (const auto &item: _expressions) { TOSTRING_LIST_ITEM_NODE(*item) }
		})
	}
}// namespace parsing
