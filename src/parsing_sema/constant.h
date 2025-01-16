#ifndef CONSTANT_H
#define CONSTANT_H

#include "numeric_constant.h"
#include "parser.h"

namespace jcc::parsing_sema {
    enum class PredefinedConstant { True, False, Nullptr };

    class AstConstant final : public AstNode {
    public:
        using Value = std::variant<
                std::unique_ptr<AstIntegerConstant>, PredefinedConstant>;

    private:
        Value m_Value;
    };

    [[nodiscard]]
    AstNodePtr ParseConstant(tokenizer::Token &token);
}// namespace jcc::parsing_sema

#endif//CONSTANT_H
