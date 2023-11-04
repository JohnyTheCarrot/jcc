//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTFUNCTIONSPECIFIER_H
#define JCC_ASTFUNCTIONSPECIFIER_H

#include "../../AstNode.h"

namespace parsing {

    struct AstFunctionSpecifier final : public AstNode {
        enum class FunctionSpecifierType {
            Inline,
            Noreturn
        };

        AstFunctionSpecifier(FunctionSpecifierType functionSpecifier)
            : AstNode(AstNodeType::FunctionSpecifier, Hierarchies::Declaration)
            , _functionSpecifier(functionSpecifier)
        {}

        [[nodiscard]]
        static std::optional<AstFunctionSpecifier> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        FunctionSpecifierType _functionSpecifier;
    };

} // parsing

#endif //JCC_ASTFUNCTIONSPECIFIER_H
