//
// Created by johny on 11/12/23.
//

#ifndef JCC_ASTITERATIONSTATEMENT_H
#define JCC_ASTITERATIONSTATEMENT_H

#include "../../AstNode.h"

namespace parsing {

	struct AstWhileIterationStatement final : public AstNode {
		AstWhileIterationStatement(const SpanOld &span, AstNode::Ptr &&condition, AstNode::Ptr &&statement, bool isDoWhile)
			: AstNode(AstNodeType::WhileIterationStatement)
			, _condition{std::move(condition)}
			, _statement{std::move(statement)}
			, _isDoWhile{isDoWhile} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstWhileIterationStatement> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstNode::Ptr _condition;
		AstNode::Ptr _statement;
		bool _isDoWhile;
	};

	struct AstForIterationStatement final : public AstNode {
		AstForIterationStatement(
				const SpanOld &span, AstNode::Ptr setUp, AstNode::Ptr check, AstNode::Ptr after, AstNode::Ptr statement
		)
			: AstNode(AstNodeType::ForIterationStatement)
			, _setUp{std::move(setUp)}
			, _check{std::move(check)}
			, _after{std::move(after)}
			, _statement{std::move(statement)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstForIterationStatement> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstNode::Ptr _setUp, _check, _after, _statement;
	};

	struct AstIterationStatement final : public AstNode {
		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;
	};

}// namespace parsing

#endif// JCC_ASTITERATIONSTATEMENT_H
