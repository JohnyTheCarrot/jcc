//
// Created by johny on 10/19/23.
//

#ifndef JCC_ASTPRIMARYEXPRESSION_H
#define JCC_ASTPRIMARYEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstPrimaryExpression final : public AstNode {
        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);
    };

} // parsing

#endif //JCC_ASTPRIMARYEXPRESSION_H