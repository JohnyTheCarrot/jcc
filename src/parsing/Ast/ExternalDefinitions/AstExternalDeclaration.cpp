//
// Created by johny on 11/12/23.
//

#include "AstExternalDeclaration.h"
#include "../../Parser.h"
#include "../Declarations/AstDeclaration.h"
#include "AstFunctionDefinition.h"

namespace parsing {
	AstNode::Ptr AstExternalDeclaration::Parse(Parser &parser) {
		std::optional<AstFunctionDefinition> functionDefinition{AstFunctionDefinition::Parse(parser)};
		if (functionDefinition)
			return std::make_unique<AstFunctionDefinition>(std::move(*functionDefinition));

		return AstDeclaration::Parse(parser);
	}

	std::string AstExternalDeclaration::ToString(size_t depth) const { NOT_APPLICABLE() }
}// namespace parsing
