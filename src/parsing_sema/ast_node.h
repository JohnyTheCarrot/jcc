#ifndef AST_NODE_H
#define AST_NODE_H

namespace jcc::parsing_sema {
    class CompilerState;
}

namespace jcc::parsing_sema {
    // TODO
    class AstNode {
    public:
        virtual ~AstNode() = default;
    };

    class AstExpression : public AstNode {
    public:
        [[nodiscard]]
        virtual llvm::Value *Codegen() = 0;
    };
}// namespace jcc::parsing_sema

#endif//AST_NODE_H
