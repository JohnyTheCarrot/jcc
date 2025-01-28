#ifndef AST_NODE_H
#define AST_NODE_H

#include <llvm/IR/Value.h>

#include "misc/Span.h"
#include "types/type.h"

namespace jcc::parsing_sema {
    class AstFloatingConstant;

    class AstShiftExpression;

    class AstAdditiveExpression;

    class AstMultiplicativeExpression;
}// namespace jcc::parsing_sema

namespace jcc::parsing_sema {
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
    };

    class AstNodeVisitor : public ExpressionVisitor {
    public:
        using ExpressionVisitor::ExpressionVisitor;
    };

    struct AstNode {
        Span m_Span;

        explicit AstNode(Span &&span);

        virtual ~AstNode() = default;
    };

    class AstExpression : public AstNode {
        types::ValueType m_Type;

    public:
        AstExpression(Span &&span, types::ValueType type);

        virtual void Accept(ExpressionVisitor *visitor) const = 0;

        [[nodiscard]]
        types::ValueType const &GetType() const;
    };

    using AstExpressionPtr = std::unique_ptr<AstExpression>;

    class AstBinaryExpression : public AstExpression {
    public:
        using AstExpression::AstExpression;

        [[nodiscard]]
        virtual AstExpressionPtr::pointer GetLhs() const noexcept = 0;

        [[nodiscard]]
        virtual AstExpressionPtr::pointer GetRhs() const noexcept = 0;
    };
}// namespace jcc::parsing_sema

#endif//AST_NODE_H
