//
// Created by johny on 11/2/23.
//

#ifndef JCC_ASTEXPRESSION_H
#define JCC_ASTEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstExpression final : public AstNode {
        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);
    };

} // parsing

#endif //JCC_ASTEXPRESSION_H
