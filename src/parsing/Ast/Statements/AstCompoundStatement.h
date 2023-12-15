//
// Created by johny on 11/8/23.
//

#ifndef JCC_ASTCOMPOUNDSTATEMENT_H
#define JCC_ASTCOMPOUNDSTATEMENT_H

#include "../../AstNode.h"
#include "AstBlockItemList.h"

namespace parsing {

	struct AstCompoundStatement final : public AstNode {
		AstCompoundStatement(const Span &span, std::optional<AstBlockItemList> blockItemList)
			: AstNode(AstNodeType::CompoundStatement)
			, _blockItemList{std::move(blockItemList)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstCompoundStatement> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::optional<AstBlockItemList> _blockItemList;
	};

}// namespace parsing

#endif//JCC_ASTCOMPOUNDSTATEMENT_H
