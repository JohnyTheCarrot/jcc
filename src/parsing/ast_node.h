#ifndef AST_NODE_H
#define AST_NODE_H

#include <llvm/IR/Value.h>

#include "misc/Span.h"
#include "types/type.h"

namespace jcc::parsing {
    class AstCastExpression;

    class AstFloatingConstant;

    class AstShiftExpression;

    class AstAdditiveExpression;

    class AstMultiplicativeExpression;

    class SpecifierQualifierList;

    class AstTypeName;
}// namespace jcc::parsing

namespace jcc::parsing {
    class AstIntegerConstant;

    class CompilerState;

    enum class ValueCategory { LValue, RValue };

    template<class T>
    concept HasValueCategory = requires(T t) {
        { T::GetValueCategory() } -> std::same_as<ValueCategory>;
    };

    class ExpressionVisitor {
    public:
        virtual ~ExpressionVisitor() = default;

        virtual void Visit(AstIntegerConstant const *astIntConst) = 0;

        virtual void
        Visit(AstMultiplicativeExpression const *astMultiplicativeExpr) = 0;

        virtual void Visit(AstAdditiveExpression const *astAdditiveExpr) = 0;

        virtual void Visit(AstShiftExpression const *astShiftExpr) = 0;

        virtual void Visit(AstFloatingConstant const *astFloatingConst) = 0;

        virtual void Visit(AstCastExpression const *astCastExpr) = 0;
    };

    class AstNodeVisitor : public ExpressionVisitor {
    public:
        using ExpressionVisitor::ExpressionVisitor;
        using ExpressionVisitor::Visit;

        virtual void
        Visit(SpecifierQualifierList const *specifierQualifierList) = 0;

        virtual void Visit(AstTypeName const *astTypeName) = 0;
    };

    struct AstNode {
        Span m_Span;

        explicit AstNode(Span span);

        virtual ~AstNode() = default;

        virtual void AcceptOnNode(AstNodeVisitor *visitor) const = 0;
    };

    class AstExpression : public AstNode {
        mutable std::optional<types::ValueType> m_DeducedType;

    public:
        explicit AstExpression(Span &&span);

        void AcceptOnNode(AstNodeVisitor *visitor) const override;

        virtual void AcceptOnExpression(ExpressionVisitor *visitor) const = 0;

        void SetDeducedType(types::ValueType type) const noexcept {
            m_DeducedType = type;
        }

        [[nodiscard]]
        types::ValueType GetDeducedType() const noexcept {
            return m_DeducedType.value();
        }
    };

    using AstExpressionPtr = std::unique_ptr<AstExpression>;

    class AstBinaryExpression : public AstExpression {
        AstExpressionPtr m_Lhs;
        AstExpressionPtr m_Rhs;
        mjolnir::Span    m_OpSpan;

    public:
        AstBinaryExpression(
                Span &&span, AstExpressionPtr lhs, AstExpressionPtr rhs,
                mjolnir::Span opSpan
        );

        [[nodiscard]]
        AstExpressionPtr::pointer GetLhs() const noexcept;

        [[nodiscard]]
        AstExpressionPtr::pointer GetRhs() const noexcept;

        [[nodiscard]]
        mjolnir::Span GetOpSpan() const noexcept;
    };
}// namespace jcc::parsing

#endif//AST_NODE_H
