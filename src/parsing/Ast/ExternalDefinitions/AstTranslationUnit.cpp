//
// Created by johny on 11/8/23.
//

#include "AstTranslationUnit.h"
#include "../../Parser.h"
#include "AstExternalDeclaration.h"

namespace parsing {
    std::optional<AstTranslationUnit> AstTranslationUnit::Parse(Parser &parser) {
        Span span{};
        std::vector<AstNode::Ptr> externalDeclarations{};
        AstNode::Ptr externalDeclaration{ AstExternalDeclaration::Parse(parser) };

        if (!externalDeclaration)
            return std::nullopt;

        span = externalDeclaration->_span;
        externalDeclarations.push_back(std::move(externalDeclaration));

        while (true) {
            externalDeclaration = AstExternalDeclaration::Parse(parser);

            if (!externalDeclaration)
                return AstTranslationUnit(span, std::move(externalDeclarations));

            span += externalDeclaration->_span;
            externalDeclarations.push_back(std::move(externalDeclaration));
        }
    }

    std::string AstTranslationUnit::ToString(size_t depth) const {
        TOSTRING_LIST(AstTranslationUnit, depth, {
            for (const auto &item: _externalDeclarations)
                TOSTRING_LIST_ITEM_NODE(*item)
        })
    }
} // parsing