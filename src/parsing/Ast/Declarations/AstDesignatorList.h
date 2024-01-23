//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTDESIGNATORLIST_H
#define JCC_ASTDESIGNATORLIST_H

#include "../../AstNode.h"
#include "AstDesignator.h"
#include <vector>

namespace parsing {

	struct AstDesignatorList final : public AstNode {
		AstDesignatorList(const SpanOld &span, std::vector<AstDesignator> &&designators)
			: AstNode(AstNodeType::DesignatorList)
			, _designators{std::move(designators)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstDesignatorList> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::vector<AstDesignator> _designators;
	};

}// namespace parsing

#endif//JCC_ASTDESIGNATORLIST_H
