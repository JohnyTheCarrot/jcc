#ifndef NUMERIC_CONSTANT_H
#define NUMERIC_CONSTANT_H

#include <random>

#include "parser.h"
#include "types/type.h"

namespace jcc::parsing_sema {
    using IntValue      = std::int64_t;
    using NumConstValue = std::variant<IntValue, double>;

    class AstIntegerConstant final : public AstExpression {
        types::IntegerType m_Type;
        IntValue           m_Value;

    public:
        AstIntegerConstant(types::IntegerType type, IntValue value) noexcept;

        [[nodiscard]]
        types::IntegerType GetType() const noexcept;

        [[nodiscard]]
        NumConstValue GetValue() const noexcept;

        [[nodiscard]]
        bool operator==(AstIntegerConstant const &other) const;

        llvm::Value *Codegen() override;

        [[nodiscard]]
        ValueCategory GetValueCategory() const noexcept override;
    };

    void PrintTo(AstIntegerConstant const &astIntConst, std::ostream *os);

    [[nodiscard]]
    std::unique_ptr<AstIntegerConstant>
    ParseNumericConstant(tokenizer::Token &token);
}// namespace jcc::parsing_sema

#endif//NUMERIC_CONSTANT_H
