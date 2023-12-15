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
		using TypeSpecifierType = std::variant<AstTypeSpecifierType, AstTypedefName>;

		AstTypeSpecifier(const Span &span, TypeSpecifierType typeSpecifierType)
			: AstNode(AstNodeType::TypeSpecifier, Hierarchies::TypeSpecifier)
			, _specifierType{typeSpecifierType} {
			this->_span = span;
		}

		[[nodiscard]]
		static std::optional<AstTypeSpecifier> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		TypeSpecifierType _specifierType;
	};

}// namespace parsing

#endif//JCC_ASTTYPESPECIFIER_H
