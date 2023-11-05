//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTINITDECLARATOR_H
#define JCC_ASTINITDECLARATOR_H

#include <memory>
#include "../../AstNode.h"
#include "AstDeclarator.h"

namespace parsing {

    struct AstInitDeclarator final : public AstNode {
        explicit AstInitDeclarator(AstDeclarator declarator)
            : AstNode(AstNodeType::InitDeclarator)
            , _declarator{ std::move(declarator) }
        {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::optional<AstDeclarator> _declarator{};
    };

} // parsing

#endif //JCC_ASTINITDECLARATOR_H
