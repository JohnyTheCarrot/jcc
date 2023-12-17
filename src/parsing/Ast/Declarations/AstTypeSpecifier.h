//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTTYPESPECIFIER_H
#define JCC_ASTTYPESPECIFIER_H

#include "../../AstNode.h"
#include "../Expressions/AstTypedefName.h"
#include <variant>

namespace parsing {

	enum class AstTypeSpecifierType {
		Void,
		Char,
		Short,
		Int,
		Long,
		Float,
		Double,
		Signed,
		Unsigned,
		Bool,
		Complex,
		Imaginary,
		Struct,
		Union,
		Enum,
		TypedefName
	};

	struct AstTypeSpecifier final : public AstNode {
		AstTypeSpecifier(const Span &span, AstTypeSpecifierType &&typeSpecifierType)
			: AstNode(AstNodeType::TypeSpecifier, Hierarchies::TypeSpecifier)
			, specifierType_{typeSpecifierType} {
			this->_span = span;
		}

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstTypeSpecifierType specifierType_;
	};

}// namespace parsing

#endif//JCC_ASTTYPESPECIFIER_H
