#ifndef CAST_EXPRESSION_HPP
#define CAST_EXPRESSION_HPP

#include "ast_node.h"
#include "type_name.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing {
    class AstCastExpression final : public AstExpression {
        AstTypeName      m_TypeName;
        AstExpressionPtr m_Expression;

    public:
        AstCastExpression(
                Span &&span, AstTypeName type, AstExpressionPtr &&expression
        );

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;

        [[nodiscard]]
        AstTypeName const &GetTypeName() const noexcept {
            return m_TypeName;
        }

        [[nodiscard]]
        AstExpressionPtr::pointer GetExpression() const noexcept {
            return m_Expression.get();
        }
    };

    [[nodiscard]]
    AstExpressionPtr ParseCastExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//CAST_EXPRESSION_HPP
