#ifndef STRUCT_DECLARATOR_LIST
#define STRUCT_DECLARATOR_LIST
#include "../../AstNode.h"
#include <memory>
#include <vector>

namespace parsing {
	struct AstStructDeclarator;

	struct AstStructDeclaratorList final : public AstNode {
		using StructDeclaratorPtr = AstNode::Ptr;
		using StructDeclaratorPtrList = std::vector<StructDeclaratorPtr>;

		AstStructDeclaratorList(const SpanOld &span, StructDeclaratorPtrList &&declarators)
			: AstNode(AstNodeType::StructDeclaratorList)
			, declarators_{std::move(declarators)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstStructDeclaratorList> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		StructDeclaratorPtrList declarators_;
	};
}// namespace parsing
#endif
