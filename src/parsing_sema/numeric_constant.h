#ifndef NUMERIC_CONSTANT_H
#define NUMERIC_CONSTANT_H

#include <random>

#include "parser.h"
#include "types/type.h"

namespace jcc::parsing_sema {
    using IntValue      = std::int64_t;
    using NumConstValue = std::variant<IntValue, double>;

    class AstIntegerConstant final : public AstNode {
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
    };

    void PrintTo(AstIntegerConstant const &astIntConst, std::ostream *os);

    namespace internal {
        [[nodiscard]]
        AstNodePtr ParseNumericConstant(tokenizer::Token &token);
    }// namespace internal

    template<typename TIterator>
        requires TokenIterator<TIterator>
    class NumericConstantParser final : public Parser<TIterator> {
    public:
        using Parser<TIterator>::Parser;

        AstNodePtr ParseNumericConstant() {
            if (this->IsExhausted()) {
                return nullptr;
            }

            auto token{*this->GetCurrent()};

            return internal::ParseNumericConstant(token);
        }
    };
}// namespace jcc::parsing_sema

#endif//NUMERIC_CONSTANT_H
