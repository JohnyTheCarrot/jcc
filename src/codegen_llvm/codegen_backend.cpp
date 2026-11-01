#include "codegen_backend.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include "codegen_context.h"
#include "expression_codegen.h"
#include "parsing/ast_node.h"

namespace jcc::codegen_llvm {
    LLVMCodegenBackend::LLVMCodegenBackend()
        : m_Context{std::make_unique<LLVMCodegenContext>()} {}

    LLVMCodegenBackend::~LLVMCodegenBackend() = default;

    LLVMCodegenBackend::LLVMCodegenBackend(LLVMCodegenBackend &&) noexcept = default;
    LLVMCodegenBackend &LLVMCodegenBackend::operator=(LLVMCodegenBackend &&) noexcept = default;

    void LLVMCodegenBackend::EmitExpression(std::string const &moduleName, parsing::AstExpression const *expression) {
        if (expression == nullptr)
            return;

        auto &context{m_Context->GetContext()};
        auto &builder{m_Context->GetBuilder()};

        ExpressionCodegenVisitor visitor{context, builder};
        expression->AcceptOnExpression(&visitor);

        auto *value{visitor.GetValue()};
        if (value == nullptr)
            return;

        llvm::Module module{moduleName, context};

        llvm::FunctionType *fnType{
                llvm::FunctionType::get(value->getType(), false)
        };

        llvm::Function *fn{llvm::Function::Create(
                fnType, llvm::Function::ExternalLinkage, "test", module
        )};

        llvm::BasicBlock *entry{
                llvm::BasicBlock::Create(context, "entry", fn)
        };
        builder.SetInsertPoint(entry);
        builder.CreateRet(value);

        module.print(llvm::outs(), nullptr);
    }
} // namespace jcc::codegen_llvm
