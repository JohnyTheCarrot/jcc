//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTTYPENAME_H
#define JCC_ASTTYPENAME_H

#include <memory>
#include <utility>
#include "../../AstNode.h"
#include "AstSpecifierQualifierList.h"

namespace parsing {

    struct AstTypeName final : public AstNode {
        explicit AstTypeName(AstSpecifierQualifierList specifierQualifierList)
            : AstNode(AstNodeType::TypeName)
            , _specifierQualifierList{std::move(specifierQualifierList)} {};

        [[nodiscard]]
        static std::optional<AstTypeName> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstSpecifierQualifierList _specifierQualifierList;
    };

} // parsing

#endif //JCC_ASTTYPENAME_H
