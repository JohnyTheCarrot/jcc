//
// Created by johny on 11/8/23.
//

#include "AstStatement.h"
#include "../../Parser.h"
#include "AstCompoundStatement.h"
#include "AstExpressionStatement.h"
#include "AstIterationStatement.h"
#include "AstJumpStatement.h"
#include "AstLabeledStatement.h"
#include "AstSelectionStatement.h"
#include <memory>

namespace parsing {
	AstNode::Ptr AstStatement::Parse(Parser &parser) {
		std::optional<AstCompoundStatement> compoundStatement{AstCompoundStatement::Parse(parser)};
		if (compoundStatement)
			return std::make_unique<AstCompoundStatement>(std::move(*compoundStatement));

		std::optional<AstExpressionStatement> expressionStatement{AstExpressionStatement::Parse(parser)};
		if (expressionStatement)
			return std::make_unique<AstExpressionStatement>(std::move(*expressionStatement));

		AstNode::Ptr iterationStatement{AstIterationStatement::Parse(parser)};
		if (iterationStatement)
			return iterationStatement;

		std::optional<AstSelectionStatement> selectionStatement{AstSelectionStatement::Parse(parser)};
		if (selectionStatement)
			return std::make_unique<AstSelectionStatement>(std::move(*selectionStatement));

		std::optional<AstJumpStatement> jumpStatement{AstJumpStatement::Parse(parser)};
		if (jumpStatement)
			return std::make_unique<AstJumpStatement>(std::move(*jumpStatement));

		std::optional<AstLabeledStatement> labeledStatement{AstLabeledStatement::Parse(parser)};
		if (labeledStatement)
			return std::make_unique<AstLabeledStatement>(std::move(*labeledStatement));

		return nullptr;
	}

	std::string AstStatement::ToString(size_t depth) const { NOT_APPLICABLE() }
}// namespace parsing
