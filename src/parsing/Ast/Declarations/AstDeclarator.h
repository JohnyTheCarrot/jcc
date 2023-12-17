//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTDECLARATOR_H
#define JCC_ASTDECLARATOR_H

#include "../../AstNode.h"
#include "AstPointer.h"

namespace parsing {

	struct AstDeclarator final : public AstNode {
		AstDeclarator(std::optional<AstPointer> &&pointer, AstNode::Ptr &&directDeclarator)
			: AstNode(AstNodeType::Declarator)
			, _pointer{std::move(pointer)}
			, _directDeclarator{std::move(directDeclarator)} {}

		[[nodiscard]]
		static std::optional<AstDeclarator> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::optional<AstPointer> _pointer{};
		AstNode::Ptr _directDeclarator{};
	};

}// namespace parsing

#endif//JCC_ASTDECLARATOR_H
