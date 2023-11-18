//
// Created by johny on 11/8/23.
//

#ifndef JCC_ASTSTATEMENT_H
#define JCC_ASTSTATEMENT_H

#include "../../AstNode.h"

namespace parsing {

    struct AstStatement final : public AstNode {
        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;
    };

} // parsing

#endif //JCC_ASTSTATEMENT_H
