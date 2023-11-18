//
// Created by johny on 11/12/23.
//

#ifndef JCC_ASTITERATIONSTATEMENT_H
#define JCC_ASTITERATIONSTATEMENT_H

#include "../../AstNode.h"
#include <memory>

namespace parsing {

	struct AstWhileIterationStatement final : public AstNode {
		AstWhileIterationStatement(
				const Span &span, std::unique_ptr<AstNode> condition, std::unique_ptr<AstNode> statement, bool isDoWhile
		)
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

		std::unique_ptr<AstNode> _condition;
		std::unique_ptr<AstNode> _statement;
		bool _isDoWhile;
	};

	struct AstForIterationStatement final : public AstNode {
		AstForIterationStatement(
				const Span &span, std::unique_ptr<AstNode> setUp, std::unique_ptr<AstNode> check,
				std::unique_ptr<AstNode> after
		)
			: AstNode(AstNodeType::ForIterationStatement)
			, _setUp{std::move(setUp)}
			, _check{std::move(check)}
			, _after{std::move(after)} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstForIterationStatement> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::unique_ptr<AstNode> _setUp, _check, _after;
	};

	struct AstIterationStatement final : public AstNode {
		[[nodiscard]]
		static std::unique_ptr<AstNode> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;
	};

}// namespace parsing

#endif// JCC_ASTITERATIONSTATEMENT_H
