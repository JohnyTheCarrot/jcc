#include "llvm_types.h"

#include <cassert>
#include <stdexcept>
#include <variant>

#include "diagnostics/diagnostics.h"

namespace jcc::codegen_llvm {
    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::BitInteger const &bitInteger) {
        return llvm::Type::getIntNTy(context, bitInteger.GetBitWidth());
    }

    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::StandardIntegerType type) {
        using parsing::types::StandardIntegerType;
        switch (type) {
            case StandardIntegerType::Bool:
                return llvm::Type::getInt1Ty(context);
            case StandardIntegerType::Char:
                return llvm::Type::getInt8Ty(context);
            case StandardIntegerType::Short:
                return llvm::Type::getInt16Ty(context);
            case StandardIntegerType::Int:
                return llvm::Type::getInt32Ty(context);
            case StandardIntegerType::Long:
            case StandardIntegerType::LongLong:
                return llvm::Type::getInt64Ty(context);
            default:
                assert(false);
                return nullptr;
        }
    }

    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::IntegerType const &integerType) {
        using parsing::types::StandardIntegerType;
        using parsing::types::BitInteger;

        auto const &varType{integerType.GetType()};
        if (std::holds_alternative<StandardIntegerType>(varType)) {
            return GetLLVMType(context, std::get<StandardIntegerType>(varType));
        }
        if (std::holds_alternative<BitInteger>(varType)) {
            return GetLLVMType(context, std::get<BitInteger>(varType));
        }

        assert(false);
        return nullptr;
    }

    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::FloatingType const &floatingType) {
        using parsing::types::StandardFloatingType;

        auto const &varType{floatingType.GetType()};
        if (std::holds_alternative<StandardFloatingType>(varType)) {
            switch (std::get<StandardFloatingType>(varType)) {
                case StandardFloatingType::Float:
                    return llvm::Type::getFloatTy(context);
                case StandardFloatingType::Double:
                    return llvm::Type::getDoubleTy(context);
                case StandardFloatingType::LongDouble:
                    return llvm::Type::getX86_FP80Ty(context);
            }
            assert(false);
            return nullptr;
        }

        throw std::runtime_error{
                "TODO: Implement GetLLVMType for non-standard floating types"
        };
    }

    llvm::Type *GetLLVMType(llvm::LLVMContext &context, parsing::types::ValueType const &valueType) {
        return std::visit([&context](auto &&arg) {
            return GetLLVMType(context, arg);
        }, valueType.GetType());
    }

    llvm::Value *CastValue(
            llvm::IRBuilder<> &builder,
            llvm::Value *value,
            parsing::types::ValueType const &from,
            parsing::types::ValueType const &to
    ) {
        auto &context = builder.getContext();

        if (from == to)
            return value;

        if (from.IsInteger() && to.IsInteger()) {
            auto const fromIntegerType{std::get<parsing::types::IntegerType>(from.GetType())};
            auto const toIntegerType{std::get<parsing::types::IntegerType>(to.GetType())};

            if (fromIntegerType.IsSigned() && toIntegerType.IsSigned()) {
                return builder.CreateSExt(value, GetLLVMType(context, toIntegerType));
            }

            return builder.CreateZExt(value, GetLLVMType(context, toIntegerType));
        }

        if (from.IsFloating() && to.IsFloating()) {
            auto const toFloatingType{std::get<parsing::types::FloatingType>(to.GetType())};

            return builder.CreateFPCast(
                    value, GetLLVMType(context, toFloatingType), "fp_cast"
            );
        }

        if (from.IsInteger() && to.IsFloating()) {
            auto const integerType{std::get<parsing::types::IntegerType>(from.GetType())};
            auto const floatingType{std::get<parsing::types::FloatingType>(to.GetType())};

            if (integerType.IsSigned()) {
                return builder.CreateSIToFP(
                        value, GetLLVMType(context, floatingType), "signed_int_to_float"
                );
            }

            return builder.CreateUIToFP(
                    value, GetLLVMType(context, floatingType), "unsigned_int_to_float"
            );
        }

        if (from.IsFloating() && to.IsInteger()) {
            auto const integerType{std::get<parsing::types::IntegerType>(to.GetType())};

            if (integerType.IsSigned()) {
                return builder.CreateFPToSI(
                        value, GetLLVMType(context, integerType), "float_to_signed_int"
                );
            }

            return builder.CreateFPToUI(
                    value, GetLLVMType(context, integerType), "float_to_unsigned_int"
            );
        }

        throw diagnostics::FatalCompilerError{};
    }
} // namespace jcc::codegen_llvm
