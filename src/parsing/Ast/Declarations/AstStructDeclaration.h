#ifndef STRUCT_DECLARATION
#define STRUCT_DECLARATION
#include "../../AstNode.h"
#include "AstSpecifierQualifierList.h"
#include "AstStructDeclaratorList.h"
#include <memory>

namespace parsing {
	struct AstStructDeclaration final : public AstNode {
		AstStructDeclaration(
				const Span &span, AstSpecifierQualifierList &&specifierQualifierList,
				std::optional<AstStructDeclaratorList> &&declaratorList
		)
			: AstNode(AstNodeType::StructDeclaration)
			, specifierQualifierList_{std::move(specifierQualifierList)}
			, structDeclaratorList_{std::move(declaratorList)} {
			this->_span = span;
		}

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstSpecifierQualifierList specifierQualifierList_;
		std::optional<AstStructDeclaratorList> structDeclaratorList_;
	};
}// namespace parsing
#endif
