//
// Created by johny on 10/19/23.
//

#ifndef JCC_ASTCONSTANTEXPRESSION_H
#define JCC_ASTCONSTANTEXPRESSION_H

#include "../../AstNode.h"
#include "AstIdentifierExpression.h"
#include "../../../tokenizer.h"


namespace parsing {

    using AstConstantValue = std::variant<IntegerLiteralTokenValue, double>;

    struct AstConstantExpression final : parsing::AstNode {
        explicit AstConstantExpression(AstConstantValue);

        static std::optional<AstConstantExpression> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstConstantValue _value;
    };

} // parsing

#endif //JCC_ASTCONSTANTEXPRESSION_H
