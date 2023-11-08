//
// Created by johny on 11/5/23.
//

#ifndef JCC_ASTDESIGNATOR_H
#define JCC_ASTDESIGNATOR_H

#include <variant>
#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstDesignator final : public AstNode {
        enum class Kind {
            ArrayIndex,
            StructOrUnionMember,
        };

        AstDesignator(const Span &span, Kind kind, std::unique_ptr<AstNode> value)
            : AstNode(AstNodeType::Designator)
            , _kind{ kind }
            , _value{ std::move(value) }
        {
            this->_span = span;
        }

        [[nodiscard]]
        static std::optional<AstDesignator> Parse(Parser &);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        Kind _kind;
        std::unique_ptr<AstNode> _value;
    };

} // parsing

#endif //JCC_ASTDESIGNATOR_H
