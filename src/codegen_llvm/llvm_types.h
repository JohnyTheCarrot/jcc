#ifndef CODEGEN_LLVM_TYPES_H
#define CODEGEN_LLVM_TYPES_H

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "parsing/types/type.h"

namespace jcc::codegen_llvm {
    [[nodiscard]]
    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::BitInteger const &bitInteger);

    [[nodiscard]]
    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::StandardIntegerType type);

    [[nodiscard]]
    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::IntegerType const &integerType);

    [[nodiscard]]
    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::FloatingType const &floatingType);

    [[nodiscard]]
    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::ValueType const &valueType);

    [[nodiscard]]
    llvm::Value *CastValue(
            llvm::IRBuilder<> &builder,
            llvm::Value *value,
            parsing::types::ValueType const &from,
            parsing::types::ValueType const &to
    );
} // namespace jcc::codegen_llvm

#endif // CODEGEN_LLVM_TYPES_H
