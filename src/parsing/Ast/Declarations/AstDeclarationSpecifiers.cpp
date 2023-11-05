//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstDeclarationSpecifiers.h"

namespace parsing {
    std::optional<AstDeclarationSpecifiers> AstDeclarationSpecifiers::Parse(Parser &parser) {
        std::vector<DeclarationSpecifier> declarationSpecifiers{};
        Span span{};

        while (true) {
            std::optional<AstStorageClassSpecifier> storageClassSpecifier{ AstStorageClassSpecifier::Parse(parser) };

            if (storageClassSpecifier.has_value()) {
                span += storageClassSpecifier->_span;
                declarationSpecifiers.emplace_back(std::move(*storageClassSpecifier));
                continue;
            }

            std::optional<AstTypeSpecifier> typeSpecifier{ AstTypeSpecifier::Parse(parser) };

            if (typeSpecifier.has_value()) {
                span += typeSpecifier->_span;
                declarationSpecifiers.emplace_back(std::move(*typeSpecifier));
                continue;
            }

            std::optional<AstTypeQualifier> typeQualifier{ AstTypeQualifier::Parse(parser) };

            if (typeQualifier.has_value()) {
                span += typeQualifier->_span;
                declarationSpecifiers.emplace_back(std::move(*typeQualifier));
                continue;
            }

            std::optional<AstFunctionSpecifier> functionSpecifier{ AstFunctionSpecifier::Parse(parser) };

            if (functionSpecifier.has_value()) {
                span += functionSpecifier->_span;
                declarationSpecifiers.emplace_back(std::move(*functionSpecifier));
                continue;
            }

            std::optional<AstAlignmentSpecifier> alignmentSpecifier{ AstAlignmentSpecifier::Parse(parser) };

            if (alignmentSpecifier.has_value()) {
                span += alignmentSpecifier->_span;
                declarationSpecifiers.emplace_back(std::move(*alignmentSpecifier));
                continue;
            }

            if (declarationSpecifiers.empty()) {
                return std::nullopt;
            }

            AstDeclarationSpecifiers declarationSpecifiersNode{ std::move(declarationSpecifiers) };
            declarationSpecifiersNode._span = span;
            return declarationSpecifiersNode;
        }
    }

    std::string AstDeclarationSpecifiers::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstDeclarationSpecifiers([" << std::endl;

        for (const auto &specifier: _declarationSpecifiers) {
            ss << tabsChildren;

            if (std::holds_alternative<AstAlignmentSpecifier>(specifier))
                ss << std::get<AstAlignmentSpecifier>(specifier).ToString(depth + 1) << std::endl;
            else if (std::holds_alternative<AstFunctionSpecifier>(specifier))
                ss << std::get<AstFunctionSpecifier>(specifier).ToString(depth + 1) << std::endl;
            else if (std::holds_alternative<AstStorageClassSpecifier>(specifier))
                ss << std::get<AstStorageClassSpecifier>(specifier).ToString(depth + 1) << std::endl;
            else if (std::holds_alternative<AstTypeSpecifier>(specifier))
                ss << std::get<AstTypeSpecifier>(specifier).ToString(depth + 1) << std::endl;
            else if (std::holds_alternative<AstTypeQualifier>(specifier))
                ss << std::get<AstTypeQualifier>(specifier).ToString(depth + 1) << std::endl;
        }

        ss << tabs << "])";

        return ss.str();
    }
} // parsing