//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTINITIALIZER_H
#define JCC_ASTINITIALIZER_H

#include "../../AstNode.h"
#include "AstInitializerList.h"

namespace parsing {

    struct AstInitializer final : public AstNode {
        AstInitializer(const Span &span, AstInitializerList initializerList)
            : AstNode(AstNodeType::Initializer)
            , _initializerList{ std::move(initializerList) }
        {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstInitializerList _initializerList;
    };

} // parsing

#endif //JCC_ASTINITIALIZER_H
