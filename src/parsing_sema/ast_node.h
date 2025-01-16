#ifndef AST_NODE_H
#define AST_NODE_H

#include <llvm/IR/Value.h>

namespace jcc::parsing_sema {
    class CompilerState;

    class AstNode {
    public:
        virtual ~AstNode() = default;
    };

    using AstNodePtr = std::unique_ptr<AstNode>;

    enum class ValueCategory { LValue, RValue };

    template<class T>
    concept HasValueCategory = requires(T t) {
        { T::GetValueCategory() } -> std::same_as<ValueCategory>;
    };

    class AstExpression : public AstNode {
    public:
        AstExpression() noexcept = default;

        [[nodiscard]]
        virtual llvm::Value *Codegen() = 0;

        [[nodiscard]]
        virtual ValueCategory GetValueCategory() const noexcept = 0;
    };

    using AstExpressionPtr = std::unique_ptr<AstExpression>;
}// namespace jcc::parsing_sema

#endif//AST_NODE_H
