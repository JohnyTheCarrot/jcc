#include "type_name.h"

#include <utility>// for move

#include "misc/Span.h"                         // for Span
#include "parsing/ast_node.h"                  // for AstNode, AstNodeVisitor
#include "parsing/types.h"                     // for SpecifierQualifierList
#include "preprocessor/preprocessor_iterator.h"// for PreprocessorIterator

namespace jcc::parsing {
    AstTypeName::AstTypeName(SpecifierQualifierList &&specifierQualifierList)
        : AstNode{specifierQualifierList.m_Span}
        , m_SpecifierQualifierList{std::move(specifierQualifierList)} {
    }

    SpecifierQualifierList const &
    AstTypeName::GetSpecifierQualifierList() const noexcept {
        return m_SpecifierQualifierList;
    }

    void AstTypeName::AcceptOnNode(AstNodeVisitor *visitor) const {
        visitor->Visit(this);
    }

    types::ValueType AstTypeName::GetType() const noexcept {
        auto const specifiers{m_SpecifierQualifierList
                                      .GetTypeSpecifierQualifier()
                                      .m_Specifiers};
        return types::ValueType{specifiers};
    }

    std::optional<AstTypeName> ParseTypeName(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return std::nullopt;

        auto possiblyFollowedByDeclarator{false};
        auto specifierQualifierList{ParseSpecifierQualifierList(
                current, end, possiblyFollowedByDeclarator
        )};
        if (!specifierQualifierList.has_value())
            return std::nullopt;

        if (!possiblyFollowedByDeclarator)
            return AstTypeName{std::move(specifierQualifierList.value())};

        // TODO: declarator
        return std::nullopt;
    }
}// namespace jcc::parsing
