//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTDECLARATIONSPECIFIERS_H
#define JCC_ASTDECLARATIONSPECIFIERS_H

#include "../../AstNode.h"
#include "AstAlignmentSpecifier.h"
#include "AstFunctionSpecifier.h"
#include "AstStorageClassSpecifier.h"
#include "AstTypeQualifier.h"
#include "AstTypeSpecifier.h"
#include <memory>
#include <variant>
#include <vector>

namespace parsing {

	struct AstDeclarationSpecifiers final : public AstNode {
		using DeclarationSpecifier = std::variant<
				AstAlignmentSpecifier, AstFunctionSpecifier, AstStorageClassSpecifier, AstNode::Ptr, AstTypeQualifier>;

		AstDeclarationSpecifiers(std::vector<DeclarationSpecifier> &&declarationSpecifiers)
			: AstNode(AstNodeType::DeclarationSpecifiers, Hierarchies::Declaration)
			, _declarationSpecifiers{std::move(declarationSpecifiers)} {}

		[[nodiscard]]
		static std::optional<AstDeclarationSpecifiers> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::vector<DeclarationSpecifier> _declarationSpecifiers;
	};

}// namespace parsing

#endif//JCC_ASTDECLARATIONSPECIFIERS_H
