//
// Created by johny on 10/19/23.
//

#ifndef JCC_ASTCONSTANT_H
#define JCC_ASTCONSTANT_H

#include "../AstNode.h"
#include "AstIdentifier.h"
#include "../../tokenizer.h"


namespace parsing {

    using AstConstantValue = std::variant<IntegerLiteralTokenValue, double>;

    struct AstConstant final : parsing::AstNode {
        explicit AstConstant(AstConstantValue);

        static std::optional<AstConstant> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstConstantValue _value;
    };

} // parsing

#endif //JCC_ASTCONSTANT_H
