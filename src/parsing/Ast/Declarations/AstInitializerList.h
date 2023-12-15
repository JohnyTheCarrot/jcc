//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTINITIALIZERLIST_H
#define JCC_ASTINITIALIZERLIST_H

#include "../../AstNode.h"
#include "AstDesignation.h"

namespace parsing {

	struct AstInitializerList final : public AstNode {
		struct InitializerListItem {
			std::optional<AstDesignation> _designation;
			AstNode::Ptr _initializer;

			[[nodiscard]]
			std::string ToString(size_t depth) const;
		};

		AstInitializerList(const Span &span, std::vector<InitializerListItem> listItems)
			: AstNode(AstNodeType::InitializerList)
			, _listItems{std::move(listItems)} {}

		[[nodiscard]]
		static std::optional<AstInitializerList> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::vector<InitializerListItem> _listItems;
	};

}// namespace parsing

#endif//JCC_ASTINITIALIZERLIST_H
