//
// Created by johny on 10/18/23.
//

#ifndef JCC_ASTNODE_H
#define JCC_ASTNODE_H

#include "../../../libs/magic_enum/magic_enum.hpp"
#include "../Span.h"
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

std::ostream &WriteStringExact(std::ostream &out, const std::string &str);

#define TODO()                                                                                                         \
	{                                                                                                                  \
		std::cerr << "TODO: " << __FILE__ << ":" << __LINE__ << std::endl;                                             \
		std::exit(1);                                                                                                  \
		__builtin_unreachable();                                                                                       \
	}
#define NOT_APPLICABLE()                                                                                               \
	{                                                                                                                  \
		std::cerr << "NOT_APPLICABLE HIT AT " << __FILE__ << ":" << __LINE__ << " (" << __FUNCTION__ << ')'            \
				  << std::endl;                                                                                        \
		std::exit(1);                                                                                                  \
	}

#define TOSTRING_ENUM(node, enumValue)                                                                                 \
	std::stringstream ss;                                                                                              \
	ss << #node "(" << magic_enum::enum_name(enumValue) << ')';                                                        \
	return ss.str();

#define TOSTRING_LIST_ITEM_NODE(field) ss << tabsChildren << (field).ToString((depth) + 1) << std::endl;

#define TOSTRING_LIST_ITEM_ENUM(field) ss << tabsChildren << magic_enum::enum_name(field) << std::endl;

#define TOSTRING_FIELD_NODE(fieldName, field)                                                                          \
	ss << tabsChildren << fieldName ": " << (field).ToString(depth + 1) << std::endl;

#define TOSTRING_FIELD_ENUM(fieldName, field)                                                                          \
	ss << tabsChildren << fieldName ": " << magic_enum::enum_name(field) << std::endl;

#define TOSTRING_FIELD_BOOL(fieldName, value)                                                                          \
	ss << tabsChildren << fieldName ": " << ((value) ? "true" : "false") << std::endl;

#define TOSTRING_FIELD_DIRECT(fieldName, value) ss << tabsChildren << fieldName ": " << (value) << std::endl;

#define TOSTRING_FIELD_STR(fieldName, value)                                                                           \
	{                                                                                                                  \
		ss << tabsChildren << fieldName ": ";                                                                          \
		WriteStringExact(ss, value);                                                                                   \
		ss << std::endl                                                                                                \
	};

#define TOSTRING_FIELDS(node, depth, code)                                                                             \
	{                                                                                                                  \
		std::stringstream ss;                                                                                          \
                                                                                                                       \
		std::string tabs{Indent(depth)};                                                                               \
		std::string tabsChildren{Indent((depth) + 1)};                                                                 \
                                                                                                                       \
		ss << #node " {" << std::endl;                                                                                 \
		code ss << tabs << '}';                                                                                        \
		return ss.str();                                                                                               \
	}

#define TOSTRING_ONE_FIELD_DIRECT(node, depth, field)                                                                  \
	{                                                                                                                  \
		std::stringstream ss;                                                                                          \
		ss << #node "(";                                                                                               \
		ss << field;                                                                                                   \
		ss << ')';                                                                                                     \
		return ss.str();                                                                                               \
	}

#define TOSTRING_ONE_FIELD_STR(node, depth, field)                                                                     \
	{                                                                                                                  \
		std::stringstream ss;                                                                                          \
		ss << #node "(";                                                                                               \
		WriteStringExact(ss, field);                                                                                   \
		ss << ')';                                                                                                     \
		return ss.str();                                                                                               \
	}

#define TOSTRING_ONE_FIELD_ENUM(node, depth, field)                                                                    \
	{                                                                                                                  \
		std::stringstream ss;                                                                                          \
		ss << #node "(";                                                                                               \
		ss << magic_enum::enum_name(field);                                                                            \
		ss << ')';                                                                                                     \
		return ss.str();                                                                                               \
	}

#define TOSTRING_ONE_FIELD_NODE(node, depth, field)                                                                    \
	{                                                                                                                  \
		std::stringstream ss;                                                                                          \
		std::string tabs{Indent(depth)};                                                                               \
		std::string tabsChildren{Indent((depth) + 1)};                                                                 \
		ss << #node "(" << std::endl;                                                                                  \
		ss << tabsChildren << (field).ToString((depth) + 1) << std::endl;                                              \
		ss << tabs << ')';                                                                                             \
		return ss.str();                                                                                               \
	}

#define TOSTRING_LIST(node, depth, code)                                                                               \
	{                                                                                                                  \
		std::stringstream ss;                                                                                          \
		std::string tabs{Indent(depth)};                                                                               \
		std::string tabsChildren{Indent((depth) + 1)};                                                                 \
		ss << #node "([" << std::endl;                                                                                 \
		code ss << tabs << "])";                                                                                       \
		return ss.str();                                                                                               \
	}

class Parser;

namespace parsing {
	enum class AstNodeType {
		Identifier,
		Constant,
		StringLiteral,
		PostfixExpression,
		UnaryExpression,
		CastExpression,
		TypeSpecifier,
		TypeQualifier,
		SpecifierQualifierList,
		TypeName,
		MultiplicativeExpression,
		AdditiveExpression,
		ShiftExpression,
		RelationalExpression,
		EqualityExpression,
		AndExpression,
		ExclusiveOr,
		InclusiveOr,
		LogicalAnd,
		LogicalOr,
		ConditionalExpression,
		AssignmentExpression,
		Expression,
		StaticAssertDeclaration,
		StorageClassSpecifier,
		FunctionSpecifier,
		AlignmentSpecifier,
		DeclarationSpecifiers,
		TypeQualifierList,
		Pointer,
		Declarator,
		InitDeclarator,
		InitDeclaratorList,
		Declaration,
		DirectDeclarator,
		DirectAbstractDeclarator,
		ParameterDeclaration,
		ParameterList,
		ParameterTypeList,
		Designator,
		DesignatorList,
		Designation,
		InitializerList,
		Initializer,
		BlockItem,
		CompoundStatement,
		TranslationUnit,
		FunctionDefinition,
		ExpressionStatement,
		WhileIterationStatement,
		ForIterationStatement,
		ArgumentExpressionList,
		SelectionStatement,
		JumpStatement,
		LabeledStatement,
		TypedefName,
	};

	struct AstNode {
		// todo: add span to constructor
		AstNode(AstNodeType type, int hierarchyLevel)
			: _type{type}
			, _span{}
			, _hierarchyLevel{hierarchyLevel} {};

		explicit AstNode(AstNodeType type)
			: AstNode(type, Hierarchies::NotApplicable){};

		using Ptr = std::unique_ptr<AstNode>;

		AstNodeType _type;
		Span _span;
		int _hierarchyLevel;

		// Hierarchy levels
		struct Hierarchies {
			// Expression hierarchy
			static constexpr int PrimaryExpression{0};
			static constexpr int PostfixExpression{1};
			static constexpr int UnaryExpression{2};
			static constexpr int CastExpression{3};
			static constexpr int MultiplicativeExpression{4};
			static constexpr int AdditiveExpression{5};
			static constexpr int ShiftExpression{6};
			static constexpr int RelationalExpression{7};
			static constexpr int EqualityExpression{8};
			static constexpr int AndExpression{9};
			static constexpr int ExclusiveOrExpression{10};
			static constexpr int InclusiveOrExpression{11};
			static constexpr int LogicalAndExpression{12};
			static constexpr int LogicalOrExpression{13};
			static constexpr int ConditionalExpression{14};
			static constexpr int AssignmentExpression{15};

			// TODO: Declaration hierarchy
			static constexpr int NotApplicable{-1};
			static constexpr int Declaration{0};
			static constexpr int TypeSpecifier{0};
			static constexpr int TypeQualifier{0};
			static constexpr int SpecifierQualifierList{1};
			static constexpr int TypeName{2};
		};

		virtual ~AstNode() = default;

		[[nodiscard]]
		virtual std::string ToString(size_t depth) const = 0;

		static std::string Indent(size_t depth);
	};
}// namespace parsing

#endif// JCC_ASTNODE_H
