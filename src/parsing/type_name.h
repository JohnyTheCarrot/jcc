#ifndef TYPE_NAME_H
#define TYPE_NAME_H

#include <optional>// for optional

#include "ast_node.h"          // for AstNode
#include "parsing/types/type.h"// for ValueType
#include "types.h"             // for SpecifierQualifierList

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    class AstTypeName final : public AstNode {
        SpecifierQualifierList m_SpecifierQualifierList;
        // TODO: abstract declarator

    public:
        explicit AstTypeName(SpecifierQualifierList &&specifierQualifierList);

        [[nodiscard]]
        SpecifierQualifierList const &
        GetSpecifierQualifierList() const noexcept;

        void AcceptOnNode(AstNodeVisitor *visitor) const override;

        [[nodiscard]]
        types::ValueType GetType() const noexcept;
    };

    [[nodiscard]]
    std::optional<AstTypeName> ParseTypeName(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//TYPE_NAME_H
