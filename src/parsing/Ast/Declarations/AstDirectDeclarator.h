//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTDIRECTDECLARATOR_H
#define JCC_ASTDIRECTDECLARATOR_H

#include "../../AstNode.h"
#include "AstTypeQualifierList.h"

namespace parsing {

    struct AstDirectDeclarator final : public AstNode {
        enum class Kind {
            Array,
            Function,
        };

        AstDirectDeclarator()
            : AstNode(AstNodeType::DirectDeclarator)
            , _typeQualifierList{ std::nullopt }
            , _assignmentExpression{ nullptr }
            , _parameterTypeList{ nullptr }
            , _lhs{ nullptr }
            , _isVLA{ false }
            , _isStatic{ false }
            , _kind{}
        {}

        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        Kind _kind;
        AstNode::Ptr _lhs;
        bool _isStatic;
        bool _isVLA;
        std::optional<AstTypeQualifierList> _typeQualifierList;
        AstNode::Ptr _assignmentExpression; // could be absent if _isStatic is false
        AstNode::Ptr _parameterTypeList;
    };

} // parsing

#endif //JCC_ASTDIRECTDECLARATOR_H
