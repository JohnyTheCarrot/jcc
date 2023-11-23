//
// Created by johny on 11/22/23.
//

#include "AstAbstractDeclarator.h"
#include "../../Parser.h"
#include "AstDirectAbstractDeclarator.h"

namespace parsing {
	std::optional<AstAbstractDeclarator> AstAbstractDeclarator::Parse(Parser &parser) {
		std::optional<AstPointer> pointer{AstPointer::Parse(parser)};
		AstNode::Ptr directAbstractDeclarator{AstDirectAbstractDeclarator::Parse(parser)};

		if (!directAbstractDeclarator && !pointer) {
			return std::nullopt;
		}

		Span span;
		if (pointer) {
			span = pointer->_span + directAbstractDeclarator->_span;
		} else {
			span = directAbstractDeclarator->_span;
		}

		AstAbstractDeclarator declarator{std::move(pointer), std::move(directAbstractDeclarator)};
		declarator._span = span;
		return declarator;
	}

	std::string AstAbstractDeclarator::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstDeclarator, depth, {
			if (_pointer) TOSTRING_FIELD_NODE("pointer", *_pointer)

			TOSTRING_FIELD_NODE("directAbstractDeclarator", *_directAbstractDeclarator)
		})
	}
}// namespace parsing
