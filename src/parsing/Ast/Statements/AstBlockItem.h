//
// Created by johny on 11/8/23.
//

#ifndef JCC_ASTBLOCKITEM_H
#define JCC_ASTBLOCKITEM_H

#include "../../AstNode.h"

namespace parsing {

	struct AstBlockItem final : public AstNode {
		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;
	};

}// namespace parsing

#endif//JCC_ASTBLOCKITEM_H
