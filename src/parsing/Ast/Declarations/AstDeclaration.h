//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTDECLARATION_H
#define JCC_ASTDECLARATION_H

#include "../../AstNode.h"
#include "AstDeclarationSpecifiers.h"
#include "AstInitDeclaratorList.h"

namespace parsing {

	struct AstDeclaration final : public AstNode {
		AstDeclaration(
				std::unique_ptr<AstDeclarationSpecifiers> declarationSpecifiers,
				std::optional<AstInitDeclaratorList> initDeclaratorList
		)
			: AstNode(AstNodeType::Declaration)
			, _declarationSpecifiers{std::move(declarationSpecifiers)}
			, _initDeclaratorList{std::move(initDeclaratorList)} {}

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::unique_ptr<AstDeclarationSpecifiers> _declarationSpecifiers;
		std::optional<AstInitDeclaratorList> _initDeclaratorList;
	};

}// namespace parsing

#endif//JCC_ASTDECLARATION_H
