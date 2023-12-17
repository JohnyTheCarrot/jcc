#ifndef STRUCT_DECLARATOR
#define STRUCT_DECLARATOR

#include "../../AstNode.h"
#include "AstDeclarator.h"
#include <memory>
#include <optional>

namespace parsing {
	struct AstStructDeclarator final : public AstNode {
		AstStructDeclarator(
				const Span &span, std::optional<AstDeclarator> &&declarator, AstNode::Ptr &&constantExpression
		)
			: AstNode(AstNodeType::StructDeclarator)
			, declarator_{std::move(declarator)}
			, constantExpression_{std::move(constantExpression)} {
			this->_span = span;
		}

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::optional<AstDeclarator> declarator_;
		AstNode::Ptr constantExpression_;
	};
}// namespace parsing

#endif
