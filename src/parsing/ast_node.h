#ifndef AST_NODE_H
#define AST_NODE_H

#include <concepts>// for same_as
#include <memory>  // for unique_ptr
#include <optional>// for optional

#include "misc/Span.h"     // for Span
#include "mjolnir/span.hpp"// for Span
#include "types/type.h"    // for ValueType

namespace jcc::parsing {
#pragma region Forward Declarations
    class AstEqualityExpression;
    class AstRelationalExpression;
    class AstCastExpression;
    class AstFloatingConstant;
    class AstShiftExpression;
    class AstAdditiveExpression;
    class AstMultiplicativeExpression;
    class SpecifierQualifierList;
    class AstTypeName;
    class AstIntegerConstant;
    class AstBitwiseAndExpression;
    class AstBitwiseXorExpression;
    class AstBitwiseOrExpression;
    class AstLogicalAndExpression;

#pragma endregion

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

        virtual void Visit(AstRelationalExpression const *astRelationalExpr
        ) = 0;

        virtual void Visit(AstEqualityExpression const *astEqualityExpression
        ) = 0;

        virtual void Visit(AstBitwiseAndExpression const *astBitwiseAndExpr
        ) = 0;

        virtual void Visit(AstBitwiseXorExpression const *astBitwiseXorExpr
        ) = 0;

        virtual void Visit(AstBitwiseOrExpression const *astBitwiseOrExpr) = 0;

        virtual void Visit(AstLogicalAndExpression const *astLogicalAndExpr
        ) = 0;
    };

    class AstNodeVisitor : public ExpressionVisitor {
    public:
        using ExpressionVisitor::ExpressionVisitor;
        using ExpressionVisitor::Visit;

        virtual void Visit(SpecifierQualifierList const *specifierQualifierList
        ) = 0;

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

    class AstBooleanBinaryExpression : public AstBinaryExpression {
        mutable std::optional<types::ValueType> m_UsualArithmeticConversionType;

    public:
        using AstBinaryExpression::AstBinaryExpression;

        [[nodiscard]]
        types::ValueType GetUsualArithmeticConversionType() const;

        void SetUsualArithmeticConversionType(types::ValueType const &type
        ) const noexcept;
    };
}// namespace jcc::parsing

#endif//AST_NODE_H
