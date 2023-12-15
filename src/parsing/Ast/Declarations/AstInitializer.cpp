//
// Created by johny on 11/5/23.
//

#include "AstInitializer.h"
#include "../../Parser.h"
#include "../Expressions/AstAssignmentExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstInitializer::Parse(Parser &parser) {
		AstNode::Ptr assignmentExpression{AstAssignmentExpression::Parse(parser)};
		if (assignmentExpression)
			return assignmentExpression;

		std::optional<Token> lBrace{parser.ConsumeIfTokenIs(TokenType::LeftBrace)};
		if (!lBrace)
			return nullptr;

		std::optional<AstInitializerList> initializerList{AstInitializerList::Parse(parser)};
		if (!initializerList)
			parser.Error(lBrace->_span, "Expected to be followed by an initializer list");

		Span span{lBrace->_span + initializerList->_span};
		Token comma;
		if (parser.AdvanceIfTokenIs(TokenType::Comma, comma))
			span += comma._span;

		parser.ExpectToken(TokenType::RightBrace);
		return std::make_unique<AstInitializer>(span, std::move(*initializerList));
	}

	std::string AstInitializer::ToString(size_t depth) const {
		TOSTRING_ONE_FIELD_NODE(AstInitializer, depth, _initializerList)
	}
}// namespace parsing
