//
// Created by johny on 10/19/23.
//

#ifndef JCC_ASTPRIMARYEXPRESSION_H
#define JCC_ASTPRIMARYEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

	struct AstPrimaryExpression final : public AstNode {
		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;
	};

}// namespace parsing

#endif//JCC_ASTPRIMARYEXPRESSION_H
