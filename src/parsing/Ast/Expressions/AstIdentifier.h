//
// Created by johny on 10/18/23.
//

#ifndef JCC_ASTIDENTIFIER_H
#define JCC_ASTIDENTIFIER_H

#include <string>
#include "../../AstNode.h"

class Parser;

namespace parsing {

    struct AstIdentifier final : public parsing::AstNode {
        explicit AstIdentifier(std::string &&ident) : AstNode(AstNodeType::Identifier), _ident{std::move(ident)} {};

        static std::optional<AstIdentifier> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::string _ident;
    };

}


#endif //JCC_ASTIDENTIFIER_H
