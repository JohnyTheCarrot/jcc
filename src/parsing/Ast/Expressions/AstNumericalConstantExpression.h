//
// Created by johny on 10/19/23.
//

#ifndef JCC_ASTNUMERICALCONSTANTEXPRESSION_H
#define JCC_ASTNUMERICALCONSTANTEXPRESSION_H

#include "../../../tokenizer.h"
#include "../../AstNode.h"
#include "AstIdentifierExpression.h"

namespace parsing {

	using AstConstantValue = std::variant<IntegerLiteralTokenValue, double>;

	struct AstNumericalConstantExpression final : parsing::AstNode {
		explicit AstNumericalConstantExpression(const AstConstantValue &value)
			: AstNode(AstNodeType::Constant, Hierarchies::PrimaryExpression)
			, _value{value} {};

		static std::optional<AstNumericalConstantExpression> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstConstantValue _value;
	};

}// namespace parsing

#endif//JCC_ASTNUMERICALCONSTANTEXPRESSION_H
