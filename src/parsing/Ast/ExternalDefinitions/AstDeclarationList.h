//
// Created by johny on 11/12/23.
//

#ifndef JCC_ASTDECLARATIONLIST_H
#define JCC_ASTDECLARATIONLIST_H

#include "../../AstNode.h"

namespace parsing {

	struct AstDeclarationList final : public AstNode {
		[[nodiscard]]
		static std::optional<AstDeclarationList> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;
	};

}// namespace parsing

#endif//JCC_ASTDECLARATIONLIST_H
