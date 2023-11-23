//
// Created by johny on 11/22/23.
//

#ifndef JCC_ASTABSTRACTDECLARATOR_H
#define JCC_ASTABSTRACTDECLARATOR_H

#include "../../AstNode.h"
#include "AstPointer.h"

namespace parsing {

	struct AstAbstractDeclarator final : public AstNode {
		AstAbstractDeclarator(std::optional<AstPointer> pointer, AstNode::Ptr directAbstractDeclarator)
			: AstNode(AstNodeType::Declarator)
			, _pointer{std::move(pointer)}
			, _directAbstractDeclarator{std::move(directAbstractDeclarator)} {}

		[[nodiscard]]
		static std::optional<AstAbstractDeclarator> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::optional<AstPointer> _pointer{};
		AstNode::Ptr _directAbstractDeclarator{};
	};

}// namespace parsing

#endif//JCC_ASTABSTRACTDECLARATOR_H
