//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTTYPEQUALIFIER_H
#define JCC_ASTTYPEQUALIFIER_H

#include "../../AstNode.h"

namespace parsing {

    enum class AstTypeQualifierType {
        Const,
        Restrict,
        Volatile,
        Atomic
    };

    struct AstTypeQualifier final : public AstNode {
        explicit AstTypeQualifier(AstTypeQualifierType qualifierType)
            : AstNode(AstNodeType::TypeQualifier)
            , _qualifierType{qualifierType} {};

        [[nodiscard]]
        static std::optional<AstTypeQualifier> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstTypeQualifierType _qualifierType;
    };

} // parsing

#endif //JCC_ASTTYPEQUALIFIER_H
