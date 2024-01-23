//
// Created by johny on 11/4/23.
//

#include "AstDeclarationSpecifiers.h"
#include <sstream>

namespace parsing {
	std::optional<AstDeclarationSpecifiers> AstDeclarationSpecifiers::Parse(Parser &parser) {
		std::vector<DeclarationSpecifier> declarationSpecifiers{};
		SpanOld span{};

		while (true) {
			std::optional<AstStorageClassSpecifier> storageClassSpecifier{AstStorageClassSpecifier::Parse(parser)};

			if (storageClassSpecifier.has_value()) {
				span += storageClassSpecifier->_span;
				declarationSpecifiers.emplace_back(std::move(*storageClassSpecifier));
				continue;
			}

			AstNode::Ptr typeSpecifier{AstTypeSpecifier::Parse(parser)};

			if (typeSpecifier) {
				span += typeSpecifier->_span;
				declarationSpecifiers.emplace_back(std::move(typeSpecifier));
				continue;
			}

			std::optional<AstTypeQualifier> typeQualifier{AstTypeQualifier::Parse(parser)};

			if (typeQualifier.has_value()) {
				span += typeQualifier->_span;
				declarationSpecifiers.emplace_back(std::move(*typeQualifier));
				continue;
			}

			std::optional<AstFunctionSpecifier> functionSpecifier{AstFunctionSpecifier::Parse(parser)};

			if (functionSpecifier.has_value()) {
				span += functionSpecifier->_span;
				declarationSpecifiers.emplace_back(std::move(*functionSpecifier));
				continue;
			}

			std::optional<AstAlignmentSpecifier> alignmentSpecifier{AstAlignmentSpecifier::Parse(parser)};

			if (alignmentSpecifier.has_value()) {
				span += alignmentSpecifier->_span;
				declarationSpecifiers.emplace_back(std::move(*alignmentSpecifier));
				continue;
			}

			if (declarationSpecifiers.empty()) {
				return std::nullopt;
			}

			AstDeclarationSpecifiers declarationSpecifiersNode{std::move(declarationSpecifiers)};
			declarationSpecifiersNode._span = span;
			return declarationSpecifiersNode;
		}
	}

	std::string AstDeclarationSpecifiers::ToString(size_t depth) const {
		TOSTRING_LIST(AstDeclarationSpecifiers, depth, {
			for (const auto &specifier: _declarationSpecifiers) {
				if (std::holds_alternative<AstAlignmentSpecifier>(specifier))
					TOSTRING_LIST_ITEM_NODE(std::get<AstAlignmentSpecifier>(specifier))
				else if (std::holds_alternative<AstFunctionSpecifier>(specifier))
					TOSTRING_LIST_ITEM_NODE(std::get<AstFunctionSpecifier>(specifier))
				else if (std::holds_alternative<AstStorageClassSpecifier>(specifier))
					TOSTRING_LIST_ITEM_NODE(std::get<AstStorageClassSpecifier>(specifier))
				else if (std::holds_alternative<AstNode::Ptr>(specifier))
					TOSTRING_LIST_ITEM_NODE(*std::get<AstNode::Ptr>(specifier))
				else if (std::holds_alternative<AstTypeQualifier>(specifier))
					TOSTRING_LIST_ITEM_NODE(std::get<AstTypeQualifier>(specifier))
			}
		})
	}
}// namespace parsing
