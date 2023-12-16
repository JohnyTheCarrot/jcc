#ifndef STRUCT_OR_UNION
#define STRUCT_OR_UNION

#include "../../AstNode.h"

namespace parsing {
	struct AstStructOrUnion final : public AstNode {
		enum class Kind { Struct, Union };

		AstStructOrUnion(const Span &span, Kind kind)
			: AstNode(AstNodeType::StructOrUnion)
			, kind_{kind} {
			this->_span = span;
		}

		static std::optional<AstStructOrUnion> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		Kind kind_;
	};
}// namespace parsing

#endif
