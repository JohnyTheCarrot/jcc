//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTPOINTER_H
#define JCC_ASTPOINTER_H

#include <optional>
#include "AstTypeQualifierList.h"

namespace parsing {

    struct AstPointer final : public AstNode {
        AstPointer()
            : AstNode(AstNodeType::Pointer)
            , _typeQualifierList{ std::nullopt }
            , _nextPointer{ nullptr }
        {}

        [[nodiscard]]
        static std::optional<AstPointer> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::optional<AstTypeQualifierList> _typeQualifierList{};
        std::unique_ptr<AstPointer> _nextPointer{};
    };

} // parsing

#endif //JCC_ASTPOINTER_H
