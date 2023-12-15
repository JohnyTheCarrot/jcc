//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTPARAMETERDECLARATION_H
#define JCC_ASTPARAMETERDECLARATION_H

#include "../../AstNode.h"
#include "AstAbstractDeclarator.h"
#include "AstDeclarationSpecifiers.h"
#include "AstDeclarator.h"

namespace parsing {

	struct AstParameterDeclaration final : public AstNode {
		using Declarator = std::variant<AstDeclarator, std::optional<AstAbstractDeclarator>>;

		AstParameterDeclaration(AstDeclarationSpecifiers specifiers, Declarator declarator)
			: AstNode(AstNodeType::ParameterDeclaration)
			, _declarationSpecifiers{std::move(specifiers)}
			, _declarator{std::move(declarator)} {}

		[[nodiscard]]
		static std::optional<AstParameterDeclaration> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstDeclarationSpecifiers _declarationSpecifiers;
		Declarator _declarator;
		// todo: abstract declarators
	};

}// namespace parsing

#endif//JCC_ASTPARAMETERDECLARATION_H
