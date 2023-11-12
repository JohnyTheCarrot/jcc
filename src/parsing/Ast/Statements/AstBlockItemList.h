//
// Created by johny on 11/8/23.
//

#ifndef JCC_ASTBLOCKITEMLIST_H
#define JCC_ASTBLOCKITEMLIST_H

#include <vector>
#include "../../AstNode.h"
#include "AstBlockItem.h"

namespace parsing {

    struct AstBlockItemList final : public AstNode {
        AstBlockItemList(const Span &span, std::vector<std::unique_ptr<AstNode>> blockItem)
            : AstNode(AstNodeType::BlockItem)
            , _items{ std::move(blockItem) }
        {
            this->_span = span;
        }

        [[nodiscard]]
        static std::optional<AstBlockItemList> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::vector<std::unique_ptr<AstNode>> _items;
    };

} // parsing

#endif //JCC_ASTBLOCKITEMLIST_H
