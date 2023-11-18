//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTPARAMETERTYPELIST_H
#define JCC_ASTPARAMETERTYPELIST_H

#include "../../AstNode.h"
#include "AstParameterList.h"

namespace parsing {

    struct AstParameterTypeList final : public AstNode {
        explicit AstParameterTypeList(AstParameterList parameterList)
            : AstNode(AstNodeType::ParameterTypeList)
            , _parameterList{ std::move(parameterList) }
        {}

        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstParameterList _parameterList;
    };

} // parsing

#endif //JCC_ASTPARAMETERTYPELIST_H
