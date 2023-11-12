//
// Created by johny on 11/8/23.
//

#ifndef JCC_ASTSTATEMENT_H
#define JCC_ASTSTATEMENT_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstStatement final : public AstNode {
        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;
    };

} // parsing

#endif //JCC_ASTSTATEMENT_H
