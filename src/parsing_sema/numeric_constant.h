#ifndef NUMERIC_CONSTANT_H
#define NUMERIC_CONSTANT_H

#include <random>

#include "parser.h"
#include "types/type.h"

namespace jcc::parsing_sema {
    using IntValue      = std::int64_t;
    using NumConstValue = std::variant<IntValue, double>;

    class AstIntegerConstant final : public AstExpression {
        IntValue m_Value;

    public:
        AstIntegerConstant(types::IntegerType type, IntValue value);

        [[nodiscard]]
        IntValue GetValue() const noexcept;

        void Accept(ExpressionVisitor *visitor) const override;

        [[nodiscard]]
        bool operator==(AstIntegerConstant const &other) const;
    };

    void PrintTo(AstIntegerConstant const &astIntConst, std::ostream *os);

    [[nodiscard]]
    std::unique_ptr<AstIntegerConstant>
    ParseNumericConstant(tokenizer::Token &token);
}// namespace jcc::parsing_sema

#endif//NUMERIC_CONSTANT_H
