#ifndef AST_TYPEDEF_NAME
#define AST_TYPEDEF_NAME

#include "../../AstNode.h"

namespace parsing {
	struct AstTypedefName final : public AstNode {
		AstTypedefName(const Span &span, const std::string &ident)
			: AstNode(AstNodeType::TypedefName)
			, typeDefName_{ident} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstTypedefName> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::string typeDefName_;
	};
}// namespace parsing

#endif
