//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTINITDECLARATORLIST_H
#define JCC_ASTINITDECLARATORLIST_H

#include <vector>
#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstInitDeclaratorList final : public AstNode {
        explicit AstInitDeclaratorList(std::vector<std::unique_ptr<AstNode>> initDeclaratorList)
            : AstNode(AstNodeType::InitDeclaratorList)
            , _initDeclaratorList{ std::move(initDeclaratorList) }
        {}

        [[nodiscard]]
        static std::optional<AstInitDeclaratorList> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::vector<std::unique_ptr<AstNode>> _initDeclaratorList{};
    };

} // parsing

#endif //JCC_ASTINITDECLARATORLIST_H
