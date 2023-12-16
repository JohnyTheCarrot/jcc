#ifndef STRUCT_OR_UNION_SPECIFIER
#define STRUCT_OR_UNION_SPECIFIER

#include "../../AstNode.h"
#include "../Expressions/AstIdentifierExpression.h"
#include "AstStructDeclarationList.h"
#include "AstStructOrUnion.h"
#include <optional>

namespace parsing {
	struct AstStructOrUnionSpecifier final : public AstNode {
		AstStructOrUnionSpecifier(
				const Span &span, AstStructOrUnion structOrUnion,
				std::optional<AstIdentifierExpression> identifierExpression,
				std::optional<AstStructDeclarationList> declList
		)
			: AstNode(AstNodeType::StructOrUnionSpecifier)
			, structOrUnion_{structOrUnion}
			, identifierExpression_{identifierExpression}
			, declList_{std::move(declList)} {
			this->_span = span;
		}

		static std::optional<AstStructOrUnionSpecifier> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstStructOrUnion structOrUnion_;
		std::optional<AstIdentifierExpression> identifierExpression_;
		std::optional<AstStructDeclarationList> declList_;
	};
}// namespace parsing

#endif
