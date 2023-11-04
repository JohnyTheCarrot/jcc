//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTCASTEXPRESSION_H
#define JCC_ASTCASTEXPRESSION_H

#include <memory>
#include "../../AstNode.h"
#include "AstIdentifierExpression.h"
#include "../Declarations/AstTypeName.h"

namespace parsing {

    struct AstCastExpression final : public AstNode {
        AstCastExpression(std::optional<AstTypeName> typeName, std::unique_ptr<AstNode> expression)
            : AstNode(AstNodeType::CastExpression)
            , _typeName { std::move(typeName) }
            , _expression { std::move(expression) }
            {};

        std::optional<AstTypeName> _typeName;
        std::unique_ptr<AstNode> _expression;

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;
    };

} // parsing

#endif //JCC_ASTCASTEXPRESSION_H
