//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTDESIGNATOR_H
#define JCC_ASTDESIGNATOR_H

#include "../../AstNode.h"
#include <variant>

namespace parsing {

	struct AstDesignator final : public AstNode {
		enum class Kind {
			ArrayIndex,
			StructOrUnionMember,
		};

		AstDesignator(const SpanOld &span, Kind kind, AstNode::Ptr &&value)
			: AstNode(AstNodeType::Designator)
			, _kind{kind}
			, _value{std::move(value)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstDesignator> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		Kind _kind;
		AstNode::Ptr _value;
	};

}// namespace parsing

#endif//JCC_ASTDESIGNATOR_H
