//
// Created by johny on 11/18/23.
//

#ifndef JCC_ASTARGUMENTEXPRESSIONLIST_H
#define JCC_ASTARGUMENTEXPRESSIONLIST_H

#include "../../AstNode.h"

namespace parsing {

	struct AstArgumentExpressionList final : public AstNode {
		AstArgumentExpressionList(const SpanOld &span, std::vector<AstNode::Ptr> &&expressions)
			: AstNode(AstNodeType::ArgumentExpressionList)
			, _expressions{std::move(expressions)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstArgumentExpressionList> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::vector<AstNode::Ptr> _expressions;
	};

}// namespace parsing

#endif//JCC_ASTARGUMENTEXPRESSIONLIST_H
