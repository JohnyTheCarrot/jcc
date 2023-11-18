//
// Created by johny on 11/12/23.
//

#ifndef JCC_ASTEXTERNALDECLARATION_H
#define JCC_ASTEXTERNALDECLARATION_H

#include "../../AstNode.h"

namespace parsing {

    struct AstExternalDeclaration final : public AstNode {
        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;
    };

} // parsing

#endif //JCC_ASTEXTERNALDECLARATION_H
