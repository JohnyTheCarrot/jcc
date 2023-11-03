//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTSTRINGLITERAL_H
#define JCC_ASTSTRINGLITERAL_H

#include "../AstNode.h"

namespace parsing {

    struct AstStringLiteral final : public AstNode {
        explicit AstStringLiteral(std::string string) : AstNode(AstNodeType::StringLiteral), _string { std::move(string) } {};

        static std::optional<AstStringLiteral> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::string _string;
    };

} // parsing

#endif //JCC_ASTSTRINGLITERAL_H
