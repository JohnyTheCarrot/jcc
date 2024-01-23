//
// Created by johny on 11/5/23.
//

#include "AstDeclarator.h"
#include "../../Parser.h"
#include "AstDirectDeclarator.h"
#include <sstream>

namespace parsing {
	std::optional<AstDeclarator> AstDeclarator::Parse(Parser &parser) {
		int currentCursor{parser.GetCursor()};
		std::optional<AstPointer> pointer{AstPointer::Parse(parser)};
		AstNode::Ptr directDeclarator{AstDirectDeclarator::Parse(parser)};

		if (!directDeclarator) {
			parser.SetCursor(currentCursor);
			return std::nullopt;
		}

		SpanOld span;
		if (pointer) {
			span = pointer->_span + directDeclarator->_span;
		} else {
			span = directDeclarator->_span;
		}

		AstDeclarator declarator{std::move(pointer), std::move(directDeclarator)};
		declarator._span = span;
		return declarator;
	}

	std::string AstDeclarator::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstDeclarator, depth, {
			if (_pointer)
				TOSTRING_FIELD_NODE("pointer", *_pointer)

			TOSTRING_FIELD_NODE("directDeclarator", *_directDeclarator)
		})
	}
}// namespace parsing
