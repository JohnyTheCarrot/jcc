//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTDECLARATOR_H
#define JCC_ASTDECLARATOR_H

#include <memory>
#include "../../AstNode.h"
#include "AstPointer.h"

namespace parsing {

    struct AstDeclarator final : public AstNode {
        AstDeclarator(std::optional<AstPointer> pointer, std::unique_ptr<AstNode> directDeclarator)
            : AstNode(AstNodeType::Declarator)
            , _pointer{ std::move(pointer) }
            , _directDeclarator{ std::move(directDeclarator) }
        {}

        [[nodiscard]]
        static std::optional<AstDeclarator> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::optional<AstPointer> _pointer{};
        std::unique_ptr<AstNode> _directDeclarator{};
    };

} // parsing

#endif //JCC_ASTDECLARATOR_H
