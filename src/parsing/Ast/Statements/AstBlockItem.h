//
// Created by johny on 11/8/23.
//

#ifndef JCC_ASTBLOCKITEM_H
#define JCC_ASTBLOCKITEM_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstBlockItem final : public AstNode {
        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;
    };

} // parsing

#endif //JCC_ASTBLOCKITEM_H
