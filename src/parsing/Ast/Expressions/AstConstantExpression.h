//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTCONSTANTEXPRESSION_H
#define JCC_ASTCONSTANTEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstConstantExpression final : public AstNode {
        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;
    };

} // parsing

#endif //JCC_ASTCONSTANTEXPRESSION_H
