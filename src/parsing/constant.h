#ifndef CONSTANT_H
#define CONSTANT_H

#include "parsing/ast_node.h"// for AstExpressionPtr

namespace jcc {
    namespace tokenizer {
        struct Token;
    }// namespace tokenizer
}// namespace jcc

namespace jcc::parsing {
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
}// namespace jcc::parsing

#endif//CONSTANT_H
