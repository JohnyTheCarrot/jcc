//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTSPECIFIERQUALIFIERLIST_H
#define JCC_ASTSPECIFIERQUALIFIERLIST_H

#include "../../AstNode.h"
#include "AstTypeQualifier.h"
#include "AstTypeSpecifier.h"
#include <variant>
#include <vector>

namespace parsing {

	struct AstSpecifierQualifierList final : public AstNode {
		AstSpecifierQualifierList()
			: AstNode(AstNodeType::SpecifierQualifierList, Hierarchies::SpecifierQualifierList)
			, _list{} {};

		using SpecifierQualifier = std::variant<AstTypeSpecifier, AstTypeQualifier>;

		[[nodiscard]]
		static std::optional<AstSpecifierQualifierList> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::vector<SpecifierQualifier> _list;
	};

}// namespace parsing

#endif//JCC_ASTSPECIFIERQUALIFIERLIST_H
