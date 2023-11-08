//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTDESIGNATION_H
#define JCC_ASTDESIGNATION_H

#include "../../AstNode.h"
#include "AstDesignatorList.h"

namespace parsing {

    struct AstDesignation final : public AstNode {
        AstDesignation(const Span &span, AstDesignatorList designatorList)
            : AstNode(AstNodeType::Designation)
            , _designatorList{ std::move(designatorList) }
        {}

        [[nodiscard]]
        static std::optional<AstDesignation> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstDesignatorList _designatorList;
    };

} // parsing

#endif //JCC_ASTDESIGNATION_H
