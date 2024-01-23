#ifndef JCC_ASTLABELEDSTATEMENT_H
#define JCC_ASTLABELEDSTATEMENT_H

#include "../../AstNode.h"
#include <optional>

namespace parsing {
	struct AstLabeledStatement final : public AstNode {
		enum class Kind { Label, Case, Default };

		AstLabeledStatement(const SpanOld &span, Kind kind, AstNode::Ptr &&inner, AstNode::Ptr &&statement)
			: AstNode(AstNodeType::LabeledStatement)
			, _kind{kind}
			, _inner{std::move(inner)}
			, _statement{std::move(statement)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstLabeledStatement> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const noexcept override;

		Kind _kind;
		AstNode::Ptr _inner;
		AstNode::Ptr _statement;
	};
}// namespace parsing

#endif
