//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTALIGNMENTSPECIFIER_H
#define JCC_ASTALIGNMENTSPECIFIER_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstAlignmentSpecifier final : public AstNode {
        enum class AlignmentSpecifierType {
            Type,
            ConstantExpression
        };

        AstAlignmentSpecifier(AlignmentSpecifierType alignmentSpecifier, std::unique_ptr<AstNode> &&alignmentSpecifierValue)
            : AstNode(AstNodeType::AlignmentSpecifier, Hierarchies::Declaration)
            , _alignmentSpecifier{ alignmentSpecifier }
            , _alignmentSpecifierValue{ std::move(alignmentSpecifierValue) }
        {
            this->_span = _alignmentSpecifierValue->_span;
        }

        [[nodiscard]]
        static std::optional<AstAlignmentSpecifier> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AlignmentSpecifierType _alignmentSpecifier;
        std::unique_ptr<AstNode> _alignmentSpecifierValue;
    };

} // parsing

#endif //JCC_ASTALIGNMENTSPECIFIER_H
