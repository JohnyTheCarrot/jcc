//
// Created by johny on 11/21/23.
//

#ifndef JCC_ASTSELECTIONSTATEMENT_H
#define JCC_ASTSELECTIONSTATEMENT_H

#include "../../AstNode.h"

namespace parsing {

	struct AstSelectionStatement final : public AstNode {
		enum class Kind { If, Switch };

		AstSelectionStatement(
				const Span &span, AstNode::Ptr &&condition, AstNode::Ptr &&body, AstNode::Ptr &&elseBody, Kind kind
		) noexcept
			: AstNode(AstNodeType::SelectionStatement)
			, _condition{std::move(condition)}
			, _body{std::move(body)}
			, _elseBody{std::move(elseBody)}
			, _kind{kind} {
			this->_span = span;
		};

		[[nodiscard]]
		static std::optional<AstSelectionStatement> Parse(Parser &parser) noexcept;

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstNode::Ptr _condition;
		AstNode::Ptr _body;
		AstNode::Ptr _elseBody;
		Kind _kind;
	};

}// namespace parsing

#endif//JCC_ASTSELECTIONSTATEMENT_H
