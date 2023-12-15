//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTINITDECLARATORLIST_H
#define JCC_ASTINITDECLARATORLIST_H

#include "../../AstNode.h"
#include <vector>

namespace parsing {

	struct AstInitDeclaratorList final : public AstNode {
		explicit AstInitDeclaratorList(std::vector<AstNode::Ptr> initDeclaratorList)
			: AstNode(AstNodeType::InitDeclaratorList)
			, _initDeclaratorList{std::move(initDeclaratorList)} {}

		[[nodiscard]]
		static std::optional<AstInitDeclaratorList> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::vector<AstNode::Ptr> _initDeclaratorList{};
	};

}// namespace parsing

#endif//JCC_ASTINITDECLARATORLIST_H
