//
// Created by johny on 11/8/23.
//

#include "AstStatement.h"
#include "../../Parser.h"
#include "AstCompoundStatement.h"
#include "AstExpressionStatement.h"
#include "AstIterationStatement.h"

namespace parsing {
    std::unique_ptr<AstNode> AstStatement::Parse(Parser &parser) {
        std::optional<AstCompoundStatement> compoundStatement{ AstCompoundStatement::Parse(parser) };
        if (compoundStatement)
            return std::make_unique<AstCompoundStatement>(std::move(*compoundStatement));

        std::optional<AstExpressionStatement> expressionStatement{ AstExpressionStatement::Parse(parser) };
        if (expressionStatement)
            return std::make_unique<AstExpressionStatement>(std::move(*expressionStatement));

		std::unique_ptr<AstNode> iterationStatement{ AstIterationStatement::Parse(parser) };
		if (iterationStatement)
			return iterationStatement;

        return nullptr;
    }

    std::string AstStatement::ToString(size_t depth) const {
        NOT_APPLICABLE()
    }
} // parsing