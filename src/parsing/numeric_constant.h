#ifndef NUMERIC_CONSTANT_H
#define NUMERIC_CONSTANT_H

#include <random>

#include "parser.h"
#include "types/type.h"

namespace jcc::parsing {
    using IntValue = std::int64_t;

    class AstIntegerConstant final : public AstExpression {
        IntValue           m_Value;
        types::IntegerType m_Type;

    public:
        AstIntegerConstant(Span span, types::IntegerType type, IntValue value);

        [[nodiscard]]
        IntValue GetValue() const noexcept;

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;

        [[nodiscard]]
        bool operator==(AstIntegerConstant const &other) const;

        [[nodiscard]]
        types::IntegerType GetNumericType() const noexcept {
            return m_Type;
        }
    };

    void PrintTo(AstIntegerConstant const &astIntConst, std::ostream *os);

    using FloatingValue = double;

    class AstFloatingConstant final : public AstExpression {
        FloatingValue       m_Value;
        types::FloatingType m_Type;

    public:
        AstFloatingConstant(
                Span span, types::FloatingType type, FloatingValue value
        );

        [[nodiscard]]
        FloatingValue GetValue() const noexcept;

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;

        [[nodiscard]]
        types::FloatingType GetNumericType() const noexcept {
            return m_Type;
        }
    };

    [[nodiscard]]
    AstExpressionPtr ParseNumericConstant(tokenizer::Token &token);
}// namespace jcc::parsing

#endif//NUMERIC_CONSTANT_H
