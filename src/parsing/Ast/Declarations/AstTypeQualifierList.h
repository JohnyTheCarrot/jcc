//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTTYPEQUALIFIERLIST_H
#define JCC_ASTTYPEQUALIFIERLIST_H

#include <vector>
#include <memory>
#include "../../AstNode.h"
#include "AstTypeQualifier.h"

namespace parsing {

    struct AstTypeQualifierList final : public AstNode {
        AstTypeQualifierList(const Span &span, std::vector<AstTypeQualifier> list)
            : AstNode(AstNodeType::TypeQualifierList)
            , _list{ std::move(list) }
        {
            this->_span = span;
        }

        [[nodiscard]]
        static std::optional<AstTypeQualifierList> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::vector<AstTypeQualifier> _list;
    };

} // parsing

#endif //JCC_ASTTYPEQUALIFIERLIST_H
