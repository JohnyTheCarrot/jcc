//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTDECLARATION_H
#define JCC_ASTDECLARATION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstDeclaration final : public AstNode {
        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;
    };

} // parsing

#endif //JCC_ASTDECLARATION_H
