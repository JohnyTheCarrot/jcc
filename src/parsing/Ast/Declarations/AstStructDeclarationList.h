#ifndef STRUCT_DECLARATION_LIST
#define STRUCT_DECLARATION_LIST
#include "../../AstNode.h"
#include "AstStructDeclaration.h"
#include <memory>

namespace parsing {
	struct AstStructDeclarationList final : public AstNode {
		using StructDeclaration = AstNode::Ptr;
		using StructDeclarationList = std::vector<StructDeclaration>;

		AstStructDeclarationList(const Span &span, StructDeclarationList list)
			: AstNode(AstNodeType::StructDeclarationList)
			, list_{std::move(list)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstStructDeclarationList> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		StructDeclarationList list_;
	};
}// namespace parsing
#endif
