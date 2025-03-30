#ifndef NUMERIC_CONSTANT_H
#define NUMERIC_CONSTANT_H

#include <random>

#include "parser.h"
#include "types/type.h"

namespace jcc::parsing_sema {
    using IntValue = std::int64_t;

    class AstIntegerConstant final : public AstExpression {
        IntValue m_Value;

    public:
        AstIntegerConstant(Span span, types::IntegerType type, IntValue value);

        [[nodiscard]]
        IntValue GetValue() const noexcept;

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;

        [[nodiscard]]
        bool operator==(AstIntegerConstant const &other) const;
    };

    void PrintTo(AstIntegerConstant const &astIntConst, std::ostream *os);

    using FloatingValue = double;

    class AstFloatingConstant final : public AstExpression {
        FloatingValue m_Value;

    public:
        AstFloatingConstant(
                Span span, types::FloatingType type, FloatingValue value
        );

        [[nodiscard]]
        FloatingValue GetValue() const noexcept;

        void AcceptOnExpression(ExpressionVisitor *visitor) const override;
    };

    [[nodiscard]]
    AstExpressionPtr ParseNumericConstant(tokenizer::Token &token);
}// namespace jcc::parsing_sema

#endif//NUMERIC_CONSTANT_H
