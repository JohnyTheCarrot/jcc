//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTTYPENAME_H
#define JCC_ASTTYPENAME_H

#include "../../AstNode.h"
#include "AstAbstractDeclarator.h"
#include "AstSpecifierQualifierList.h"
#include <utility>

namespace parsing {

	struct AstTypeName final : public AstNode {
		explicit AstTypeName(
				AstSpecifierQualifierList &&specifierQualifierList,
				std::optional<AstAbstractDeclarator> &&abstractDeclarator
		)
			: AstNode(AstNodeType::TypeName, Hierarchies::TypeName)
			, _specifierQualifierList{std::move(specifierQualifierList)}
			, _abstractDeclarator{std::move(abstractDeclarator)} {};

		[[nodiscard]]
		static std::optional<AstTypeName> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstSpecifierQualifierList _specifierQualifierList;
		std::optional<AstAbstractDeclarator> _abstractDeclarator;
	};

}// namespace parsing

#endif//JCC_ASTTYPENAME_H
