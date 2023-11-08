//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTINITDECLARATOR_H
#define JCC_ASTINITDECLARATOR_H

#include <memory>
#include "../../AstNode.h"
#include "AstDeclarator.h"
#include "AstInitializer.h"

namespace parsing {

    struct AstInitDeclarator final : public AstNode {
        AstInitDeclarator(const Span &span, AstDeclarator declarator, std::unique_ptr<AstNode> initializer)
            : AstNode(AstNodeType::InitDeclarator)
            , _declarator{ std::move(declarator) }
            , _initializer{ std::move(initializer) }
        {
            this->_span = span;
        }

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstDeclarator _declarator;
        std::unique_ptr<AstNode> _initializer;
    };

} // parsing

#endif //JCC_ASTINITDECLARATOR_H
