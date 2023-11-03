//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTTYPESPECIFIER_H
#define JCC_ASTTYPESPECIFIER_H

#include "../../AstNode.h"

namespace parsing {

    enum class AstTypeSpecifierType {
        Void,
        Char,
        Short,
        Int,
        Long,
        Float,
        Double,
        Signed,
        Unsigned,
        Bool,
        Complex,
        Imaginary,
        Struct,
        Union,
        Enum,
        TypedefName
    };

    struct AstTypeSpecifier final : public AstNode {
        explicit AstTypeSpecifier(AstTypeSpecifierType typeSpecifierType)
            : AstNode(AstNodeType::TypeSpecifier)
            , _specifierType{typeSpecifierType }
        {}

        [[nodiscard]]
        static std::optional<AstTypeSpecifier> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstTypeSpecifierType _specifierType;
    };

} // parsing

#endif //JCC_ASTTYPESPECIFIER_H
