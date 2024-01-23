//
// Created by johny on 11/12/23.
//

#ifndef JCC_ASTFUNCTIONDEFINITION_H
#define JCC_ASTFUNCTIONDEFINITION_H

#include "../../AstNode.h"
#include "../Declarations/AstDeclarationSpecifiers.h"
#include "../Declarations/AstDeclarator.h"
#include "../Statements/AstCompoundStatement.h"

namespace parsing {

	struct AstFunctionDefinition final : public AstNode {
		AstFunctionDefinition(
				const SpanOld &span, AstDeclarationSpecifiers &&declarationSpecifiers, AstDeclarator &&declarator,
				AstCompoundStatement &&compoundStatement
		)
			: AstNode(AstNodeType::FunctionDefinition)
			, _declarationSpecifiers{std::move(declarationSpecifiers)}
			, _declarator{std::move(declarator)}
			, _compoundStatement{std::move(compoundStatement)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstFunctionDefinition> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstDeclarationSpecifiers _declarationSpecifiers;
		AstDeclarator _declarator;
		AstCompoundStatement _compoundStatement;
	};

}// namespace parsing

#endif//JCC_ASTFUNCTIONDEFINITION_H
