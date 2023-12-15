//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTCONSTANTEXPRESSION_H
#define JCC_ASTCONSTANTEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

	struct AstConstantExpression final : public AstNode {
		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;
	};

}// namespace parsing

#endif//JCC_ASTCONSTANTEXPRESSION_H
