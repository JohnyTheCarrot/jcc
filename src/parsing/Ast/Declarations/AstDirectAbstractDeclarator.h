//
// Created by johny on 11/22/23.
//

#ifndef JCC_ASTDIRECTABSTRACTDECLARATOR_H
#define JCC_ASTDIRECTABSTRACTDECLARATOR_H

#include "../../AstNode.h"
#include "AstAbstractDeclarator.h"
#include "AstTypeQualifierList.h"

namespace parsing {

	struct AstDirectAbstractDeclarator final : public AstNode {
		enum class Kind { Array, Function };

		AstDirectAbstractDeclarator(const Span &span, Kind kind, AstNode::Ptr &&lhs)
			: AstNode(AstNodeType::DirectAbstractDeclarator)
			, _kind{kind}
			, _lhs{std::move(lhs)} {
			this->_span = span;
		}

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		Kind _kind;
		bool _isStatic{false}, _isVLA{false};
		AstNode::Ptr _lhs;
		AstNode::Ptr _parameterTypeList{nullptr};
		AstNode::Ptr _assignmentExpression{nullptr};
		std::optional<AstAbstractDeclarator> _abstractDeclarator{std::nullopt};
		std::optional<AstTypeQualifierList> _typeQualifierList{std::nullopt};
	};

}// namespace parsing

#endif//JCC_ASTDIRECTABSTRACTDECLARATOR_H
