#include "expression_codegen.h"

#include <cassert>            // for assert
#include <llvm/IR/Constant.h> // for Constant
#include <llvm/IR/Constants.h>// for ConstantFP, ConstantInt
#include <llvm/IR/IRBuilder.h>// for IRBuilder
#include <stdexcept>          // for runtime_error
#include <variant>            // for get

#include "codegen_llvm/llvm_types.h"          // for GetLLVMType, CastValue
#include "diagnostics/variants/todo.hpp"      // for TodoError
#include "misc/Span.h"                        // for Span
#include "parsing/additive_expression.h"      // for AstAdditiveExpression
#include "parsing/bitwise_and_expression.hpp" // for AstBitwiseAndExpression
#include "parsing/bitwise_or_expression.hpp"  // for AstBitwiseOrExpression
#include "parsing/bitwise_xor_expression.hpp" // for AstBitwiseXorExpression
#include "parsing/cast_expression.hpp"        // for AstCastExpression
#include "parsing/equality_expression.hpp"    // for AstEqualityExpression
#include "parsing/logical_and_expression.hpp" // for AstLogicalAndExpression
#include "parsing/multiplicative_expression.h"// for AstMultiplicativeExpr...
#include "parsing/numeric_constant.h"         // for AstFloatingConstant
#include "parsing/parser.h"                   // for CompilerState
#include "parsing/relational_expression.hpp"  // for AstRelationalExpression
#include "parsing/shift_expression.h"         // for AstShiftExpression
#include "parsing/type_name.h"                // for AstTypeName
#include "parsing/types/type.h"               // for ValueType, IntegerType

namespace jcc::codegen_llvm {
    std::pair<llvm::Value *, llvm::Value *>
    ExpressionCodegenVisitor::PrepareOperands(
            parsing::AstBinaryExpression const *astExpr,
            parsing::types::ValueType           type
    ) {
        auto const *lhs{astExpr->GetLhs()};
        auto const *rhs{astExpr->GetRhs()};

        lhs->AcceptOnExpression(this);
        auto *lhsValue{GetValue()};

        if (lhs->GetDeducedType() != type) {
            lhsValue = CastValue(m_Builder, lhsValue, lhs->GetDeducedType(), type);
        }

        rhs->AcceptOnExpression(this);
        auto *rhsValue{GetValue()};

        if (rhs->GetDeducedType() != type) {
            rhsValue = CastValue(m_Builder, rhsValue, rhs->GetDeducedType(), type);
        }

        return {lhsValue, rhsValue};
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstIntegerConstant const *astIntConst
    ) {
        auto const value{astIntConst->GetValue()};

        auto const type{std::get<parsing::types::IntegerType>(
                astIntConst->GetDeducedType().GetType()
        )};

        auto *llvmType{GetLLVMType(m_Context, type)};

        m_Value = llvm::ConstantInt::get(llvmType, value, type.IsSigned());
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstMultiplicativeExpression const *astMultiplicativeExpr
    ) {
        auto [lhsValue, rhsValue]{PrepareOperands(
                astMultiplicativeExpr, astMultiplicativeExpr->GetDeducedType()
        )};

        auto const &type{astMultiplicativeExpr->GetDeducedType()};

        if (type.IsInteger()) {
            auto const intType{std::get<parsing::types::IntegerType>(
                    astMultiplicativeExpr->GetDeducedType().GetType()
            )};

            switch (astMultiplicativeExpr->GetOperator()) {
                case parsing::MultiplicativeOperator::Multiplication:
                    m_Value = m_Builder.CreateMul(
                            lhsValue, rhsValue, "mult_result"
                    );
                    break;
                case parsing::MultiplicativeOperator::Division:
                    if (intType.IsSigned()) {
                        m_Value = m_Builder.CreateSDiv(
                                lhsValue, rhsValue, "div_result"
                        );
                    } else {
                        m_Value = m_Builder.CreateUDiv(
                                lhsValue, rhsValue, "div_result"
                        );
                    }
                    break;
                case parsing::MultiplicativeOperator::Modulo:
                    if (intType.IsSigned()) {
                        m_Value = m_Builder.CreateSRem(
                                lhsValue, rhsValue, "rem_result"
                        );
                    } else {
                        m_Value = m_Builder.CreateURem(
                                lhsValue, rhsValue, "rem_result"
                        );
                    }
                    break;
            }
            return;
        }

        if (type.IsFloating()) {
            switch (astMultiplicativeExpr->GetOperator()) {
                case parsing::MultiplicativeOperator::Multiplication:
                    m_Value = m_Builder.CreateFMul(
                            lhsValue, rhsValue, "fp_mult_result"
                    );
                    break;
                case parsing::MultiplicativeOperator::Division:
                    m_Value = m_Builder.CreateFDiv(
                            lhsValue, rhsValue, "fp_div_result"
                    );
                    break;
                case parsing::MultiplicativeOperator::Modulo:
                    m_Value = m_Builder.CreateFRem(
                            lhsValue, rhsValue, "fp_rem_result"
                    );
                    break;
            }
            return;
        }

        throw std::runtime_error{
                "Unsupported multiplicative expression type"
        };
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstAdditiveExpression const *astAdditiveExpr
    ) {
        auto [lhsValue, rhsValue]{PrepareOperands(
                astAdditiveExpr, astAdditiveExpr->GetDeducedType()
        )};

        if (astAdditiveExpr->GetDeducedType().IsInteger()) {
            switch (astAdditiveExpr->GetOperator()) {
                case parsing::AdditiveOperator::Addition:
                    m_Value = m_Builder.CreateAdd(
                            lhsValue, rhsValue, "add_result"
                    );
                    break;
                case parsing::AdditiveOperator::Subtraction:
                    m_Value = m_Builder.CreateSub(
                            lhsValue, rhsValue, "sub_result"
                    );
                    break;
            }
            return;
        }

        if (astAdditiveExpr->GetDeducedType().IsFloating()) {
            switch (astAdditiveExpr->GetOperator()) {
                case parsing::AdditiveOperator::Addition:
                    m_Value = m_Builder.CreateFAdd(
                            lhsValue, rhsValue, "fadd_result"
                    );
                    break;
                case parsing::AdditiveOperator::Subtraction:
                    m_Value = m_Builder.CreateFSub(
                            lhsValue, rhsValue, "fsub_result"
                    );
                    break;
            }
            return;
        }

        throw std::runtime_error{"Unsupported additive expression type"};
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstShiftExpression const *astShiftExpr
    ) {
        auto [lhsValue, rhsValue]{PrepareOperands(
                astShiftExpr, astShiftExpr->GetDeducedType()
        )};

        if (astShiftExpr->GetDeducedType().IsInteger()) {
            switch (astShiftExpr->GetOperator()) {
                case parsing::ShiftOperator::Left:
                    m_Value = m_Builder.CreateShl(
                            lhsValue, rhsValue, "shl_result"
                    );
                    break;
                case parsing::ShiftOperator::Right:
                    m_Value = m_Builder.CreateAShr(
                            lhsValue, rhsValue, "shr_result"
                    );
                    break;
            }
            return;
        }

        throw std::runtime_error{"Unsupported additive expression type"};
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstFloatingConstant const *astFloatingConst
    ) {
        auto const value{astFloatingConst->GetValue()};

        auto const type{std::get<parsing::types::FloatingType>(
                astFloatingConst->GetDeducedType().GetType()
        )};

        auto *llvmType{GetLLVMType(m_Context, type)};

        m_Value = llvm::ConstantFP::get(llvmType, value);
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstCastExpression const *astCastExpr
    ) {
        auto const *expression{astCastExpr->GetExpression()};
        expression->AcceptOnExpression(this);
        auto       *value{GetValue()};
        auto const &type{astCastExpr->GetTypeName().GetType()};

        m_Value = CastValue(m_Builder, value, expression->GetDeducedType(), type);
    }

    [[nodiscard]]
    static llvm::Value *RelationalCmpFloat(
            llvm::IRBuilder<> &builder,
            llvm::Value *lhs, llvm::Value *rhs, parsing::RelationalOperator op
    ) {
        switch (op) {
            case parsing::RelationalOperator::LessThan:
                return builder.CreateFCmpOLT(lhs, rhs, "fp_lt_result");
            case parsing::RelationalOperator::GreaterThan:
                return builder.CreateFCmpOGT(lhs, rhs, "fp_gt_result");
            case parsing::RelationalOperator::LessThanOrEqual:
                return builder.CreateFCmpOLE(lhs, rhs, "fp_le_result");
            case parsing::RelationalOperator::GreaterThanOrEqual:
                return builder.CreateFCmpOGE(lhs, rhs, "fp_ge_result");
        }

        assert(false);
        return nullptr;
    }

    [[nodiscard]]
    static llvm::Value *RelationalCmpSignedInt(
            llvm::IRBuilder<> &builder,
            llvm::Value *lhs, llvm::Value *rhs, parsing::RelationalOperator op
    ) {
        switch (op) {
            case parsing::RelationalOperator::LessThan:
                return builder.CreateICmpSLT(lhs, rhs, "int_lt_result");
            case parsing::RelationalOperator::GreaterThan:
                return builder.CreateICmpSGT(lhs, rhs, "int_gt_result");
            case parsing::RelationalOperator::LessThanOrEqual:
                return builder.CreateICmpSLE(lhs, rhs, "int_le_result");
            case parsing::RelationalOperator::GreaterThanOrEqual:
                return builder.CreateICmpSGE(lhs, rhs, "int_ge_result");
        }

        assert(false);
        return nullptr;
    }

    [[nodiscard]]
    static llvm::Value *RelationalCmpUnsignedInt(
            llvm::IRBuilder<> &builder,
            llvm::Value *lhs, llvm::Value *rhs, parsing::RelationalOperator op
    ) {
        switch (op) {
            case parsing::RelationalOperator::LessThan:
                return builder.CreateICmpULT(lhs, rhs, "uint_lt_result");
            case parsing::RelationalOperator::GreaterThan:
                return builder.CreateICmpUGT(lhs, rhs, "uint_gt_result");
            case parsing::RelationalOperator::LessThanOrEqual:
                return builder.CreateICmpULE(lhs, rhs, "uint_le_result");
            case parsing::RelationalOperator::GreaterThanOrEqual:
                return builder.CreateICmpUGE(lhs, rhs, "uint_ge_result");
        }

        assert(false);
        return nullptr;
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstRelationalExpression const *astRelationalExpr
    ) {
        auto const type{astRelationalExpr->GetUsualArithmeticConversionType()};
        auto [lhsValue, rhsValue]{PrepareOperands(astRelationalExpr, type)};

        if (type.IsFloating()) {
            m_Value = RelationalCmpFloat(
                    m_Builder, lhsValue, rhsValue, astRelationalExpr->GetOperator()
            );
        } else if (type.IsInteger()) {
            auto const intType{std::get<parsing::types::IntegerType>(
                    astRelationalExpr->GetDeducedType().GetType()
            )};

            if (intType.IsSigned()) {
                m_Value = RelationalCmpSignedInt(
                        m_Builder, lhsValue, rhsValue, astRelationalExpr->GetOperator()
                );
            } else {
                m_Value = RelationalCmpUnsignedInt(
                        m_Builder, lhsValue, rhsValue, astRelationalExpr->GetOperator()
                );
            }
        } else {
            parsing::CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<diagnostics::TodoError>(
                            astRelationalExpr->m_Span.m_Source,
                            astRelationalExpr->m_Span.m_Span
                    );
        }
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstEqualityExpression const *astEqualityExpression
    ) {
        auto const type{astEqualityExpression->GetUsualArithmeticConversionType()};
        auto const [lhsValue, rhsValue]{PrepareOperands(
                astEqualityExpression, type
        )};

        if (type.IsArithmetic()) {
            if (type.IsFloating()) {
                switch (astEqualityExpression->GetOperator()) {
                    case parsing::EqualityOperator::Equal:
                        m_Value = m_Builder.CreateFCmpOEQ(
                                lhsValue, rhsValue, "fp_eq_result"
                        );
                        break;
                    case parsing::EqualityOperator::NotEqual:
                        m_Value = m_Builder.CreateFCmpONE(
                                lhsValue, rhsValue, "fp_ne_result"
                        );
                        break;
                }
            } else if (type.IsInteger()) {
                switch (astEqualityExpression->GetOperator()) {
                    case parsing::EqualityOperator::Equal:
                        m_Value = m_Builder.CreateICmpEQ(
                                lhsValue, rhsValue, "int_eq_result"
                        );
                        break;
                    case parsing::EqualityOperator::NotEqual:
                        m_Value = m_Builder.CreateICmpNE(
                                lhsValue, rhsValue, "int_ne_result"
                        );
                        break;
                }
            } else {
                parsing::CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<diagnostics::TodoError>(
                                astEqualityExpression->m_Span.m_Source,
                                astEqualityExpression->m_Span.m_Span
                        );
            }
        }
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstBitwiseAndExpression const *astBitwiseAndExpr
    ) {
        auto const type{astBitwiseAndExpr->GetDeducedType()};
        auto [lhsValue, rhsValue]{PrepareOperands(astBitwiseAndExpr, type)};

        m_Value = m_Builder.CreateAnd(lhsValue, rhsValue, "and_result");
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstBitwiseXorExpression const *astBitwiseXorExpr
    ) {
        auto const type{astBitwiseXorExpr->GetDeducedType()};
        auto [lhsValue, rhsValue]{PrepareOperands(astBitwiseXorExpr, type)};

        m_Value = m_Builder.CreateXor(lhsValue, rhsValue, "xor_result");
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstBitwiseOrExpression const *astBitwiseOrExpr
    ) {
        auto const type{astBitwiseOrExpr->GetDeducedType()};
        auto [lhsValue, rhsValue]{PrepareOperands(astBitwiseOrExpr, type)};

        m_Value = m_Builder.CreateOr(lhsValue, rhsValue, "incl_or_result");
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstLogicalAndExpression const *astLogicalAndExpr
    ) {
        auto const boolValueType{
                parsing::types::ValueType{parsing::types::IntegerType{
                        parsing::types::StandardIntegerType::Bool,
                        parsing::types::IntegerType::Signedness::Unspecified
                }}
        };

        auto [lhsValue,
              rhsValue]{PrepareOperands(astLogicalAndExpr, boolValueType)};

        m_Value = m_Builder.CreateLogicalAnd(
                lhsValue, rhsValue, "logical_and_result"
        );
    }

    llvm::Value *ExpressionCodegenVisitor::GetValue() noexcept {
        auto const copy{m_Value};
        m_Value = nullptr;

        return copy;
    }
} // namespace jcc::codegen_llvm
