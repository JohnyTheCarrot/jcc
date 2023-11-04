//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTSTRINGLITERALEXPRESSION_H
#define JCC_ASTSTRINGLITERALEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

    struct AstStringLiteralExpression final : public AstNode {
        explicit AstStringLiteralExpression(std::string string)
            : AstNode(AstNodeType::StringLiteral, Hierarchies::PrimaryExpression)
            , _string { std::move(string) } {};

        [[nodiscard]]
        static std::optional<AstStringLiteralExpression> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::string _string;
    };

} // parsing

#endif //JCC_ASTSTRINGLITERALEXPRESSION_H
