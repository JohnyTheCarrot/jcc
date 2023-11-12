//
// Created by johny on 11/8/23.
//

#include <sstream>
#include "AstBlockItemList.h"

namespace parsing {
    std::optional<AstBlockItemList> AstBlockItemList::Parse(Parser &parser) {
        Span span{};
        std::vector<std::unique_ptr<AstNode>> blockItems{};
        std::unique_ptr<AstNode> item{ AstBlockItem::Parse(parser) };
        if (!item)
            return std::nullopt;

        span += item->_span;
        blockItems.push_back(std::move(item));

        while (true) {
            item = AstBlockItem::Parse(parser);
            if (!item)
                return AstBlockItemList(span, std::move(blockItems));

            span += item->_span;
            blockItems.push_back(std::move(item));
        }
    }

    std::string AstBlockItemList::ToString(size_t depth) const {
        TOSTRING_LIST(AstBlockItem, depth, {
            for (const auto &item: _items)
                TOSTRING_LIST_ITEM_NODE(*item)
        })
    }
} // parsing