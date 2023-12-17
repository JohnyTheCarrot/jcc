//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTPARAMETERLIST_H
#define JCC_ASTPARAMETERLIST_H

#include "../../AstNode.h"
#include "AstParameterDeclaration.h"
#include <vector>

namespace parsing {

	struct AstParameterList final : public AstNode {
		explicit AstParameterList(std::vector<AstParameterDeclaration> &&declarations)
			: AstNode(AstNodeType::ParameterList)
			, _paramDeclarations{std::move(declarations)} {}

		[[nodiscard]]
		static std::optional<AstParameterList> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::vector<AstParameterDeclaration> _paramDeclarations;
	};

}// namespace parsing

#endif//JCC_ASTPARAMETERLIST_H
