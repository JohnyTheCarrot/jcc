#ifndef JCC_ASTJUMPSTATEMENT_H
#define JCC_ASTJUMPSTATEMENT_H

#include "../../AstNode.h"

namespace parsing {

	struct AstJumpStatement final : public AstNode {
		enum class Kind { Goto, Continue, Break, Return };

		AstJumpStatement(const SpanOld &span, Kind kind, AstNode::Ptr &&inner)
			: AstNode(AstNodeType::JumpStatement)
			, _kind{kind}
			, _inner{std::move(inner)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstJumpStatement> Parse(Parser &) noexcept;

		[[nodiscard]]
		std::string ToString(size_t depth) const noexcept override;

		Kind _kind;
		AstNode::Ptr _inner;
	};

}// namespace parsing

#endif
