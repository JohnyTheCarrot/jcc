//
// Created by johny on 11/8/23.
//

#ifndef JCC_ASTTRANSLATIONUNIT_H
#define JCC_ASTTRANSLATIONUNIT_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstTranslationUnit final : public AstNode {
        AstTranslationUnit(const Span &span, std::vector<std::unique_ptr<AstNode>> &&externalDeclarations)
                : AstNode(AstNodeType::TranslationUnit)
                , _externalDeclarations{std::move(externalDeclarations)} {
            this->_span = span;
        }

        [[nodiscard]]
        static std::optional<AstTranslationUnit> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::vector<std::unique_ptr<AstNode>> _externalDeclarations;
    };

} // parsing

#endif //JCC_ASTTRANSLATIONUNIT_H
