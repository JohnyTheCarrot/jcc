#ifndef CONSTANT_H
#define CONSTANT_H

#include "parser.h"

namespace jcc::parsing_sema {
    // enum class PredefinedConstant { True, False, Nullptr };

    // class AstPredefinedConstant final : public AstExpression {
    //     PredefinedConstant m_Value;
    //
    // public:
    //     explicit AstPredefinedConstant(PredefinedConstant value) noexcept;
    //
    //     [[nodiscard]]
    //     llvm::Value *Codegen() override;
    //
    //     [[nodiscard]]
    //     ValueCategory GetValueCategory() const noexcept override;
    // };

    [[nodiscard]]
    AstExpressionPtr ParseConstant(tokenizer::Token &token);
}// namespace jcc::parsing_sema

#endif//CONSTANT_H
