#include "expression_codegen.h"

#include "parsing/additive_expression.h"
#include "parsing/bitwise_and_expression.hpp"
#include "parsing/bitwise_xor_expression.hpp"
#include "parsing/cast_expression.hpp"
#include "parsing/equality_expression.hpp"
#include "parsing/multiplicative_expression.h"
#include "parsing/numeric_constant.h"
#include "parsing/relational_expression.hpp"
#include "parsing/shift_expression.h"

namespace jcc::visitors {
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
            lhsValue = CastValue(lhsValue, lhs->GetDeducedType(), type);
        }

        rhs->AcceptOnExpression(this);
        auto *rhsValue{GetValue()};

        if (rhs->GetDeducedType() != type) {
            rhsValue = CastValue(rhsValue, rhs->GetDeducedType(), type);
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

        auto *llvmType{type.GetLLVMType()};

        m_Value = llvm::ConstantInt::get(llvmType, value, type.IsSigned());
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstMultiplicativeExpression const *astMultiplicativeExpr
    ) {
        auto [lhsValue, rhsValue]{PrepareOperands(
                astMultiplicativeExpr, astMultiplicativeExpr->GetDeducedType()
        )};

        auto       &builder{parsing::CompilerState::GetInstance().GetBuilder()};
        auto const &type{astMultiplicativeExpr->GetDeducedType()};

        if (type.IsInteger()) {
            auto const intType{std::get<parsing::types::IntegerType>(
                    astMultiplicativeExpr->GetDeducedType().GetType()
            )};

            switch (astMultiplicativeExpr->GetOperator()) {
                case parsing::MultiplicativeOperator::Multiplication:
                    m_Value = builder.CreateMul(
                            lhsValue, rhsValue, "mult_result"
                    );
                    break;
                case parsing::MultiplicativeOperator::Division:
                    if (intType.IsSigned()) {
                        m_Value = builder.CreateSDiv(
                                lhsValue, rhsValue, "div_result"
                        );
                    } else {
                        m_Value = builder.CreateUDiv(
                                lhsValue, rhsValue, "div_result"
                        );
                    }
                    break;
                case parsing::MultiplicativeOperator::Modulo:
                    if (intType.IsSigned()) {
                        m_Value = builder.CreateSRem(
                                lhsValue, rhsValue, "mod_result"
                        );
                    } else {
                        m_Value = builder.CreateURem(
                                lhsValue, rhsValue, "mod_result"
                        );
                    }
                    break;
            }
            return;
        }

        if (type.IsFloating()) {
            switch (astMultiplicativeExpr->GetOperator()) {
                case parsing::MultiplicativeOperator::Multiplication:
                    m_Value = builder.CreateFMul(
                            lhsValue, rhsValue, "fmult_result"
                    );
                    return;
                case parsing::MultiplicativeOperator::Division:
                    m_Value = builder.CreateFDiv(
                            lhsValue, rhsValue, "fdiv_result"
                    );
                    return;
                case parsing::MultiplicativeOperator::Modulo:
                    assert(false &&
                           "Floating point modulo not supported, and should "
                           "have been caught by the semantic analysis.");
                    break;
            }
        }

        throw std::runtime_error{"Unsupported multiplicative expression type"};
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstAdditiveExpression const *astAdditiveExpr
    ) {
        auto [lhsValue, rhsValue]{PrepareOperands(
                astAdditiveExpr, astAdditiveExpr->GetDeducedType()
        )};

        auto &builder{parsing::CompilerState::GetInstance().GetBuilder()};

        if (astAdditiveExpr->GetDeducedType().IsInteger()) {
            switch (astAdditiveExpr->GetOperator()) {
                case parsing::AdditiveOperator::Addition:
                    m_Value =
                            builder.CreateAdd(lhsValue, rhsValue, "add_result");
                    break;
                case parsing::AdditiveOperator::Subtraction:
                    m_Value =
                            builder.CreateSub(lhsValue, rhsValue, "sub_result");
                    break;
            }
            return;
        }

        if (astAdditiveExpr->GetDeducedType().IsFloating()) {
            switch (astAdditiveExpr->GetOperator()) {
                case parsing::AdditiveOperator::Addition:
                    m_Value = builder.CreateFAdd(
                            lhsValue, rhsValue, "fadd_result"
                    );
                    break;
                case parsing::AdditiveOperator::Subtraction:
                    m_Value = builder.CreateFSub(
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
        auto [lhsValue, rhsValue]{
                PrepareOperands(astShiftExpr, astShiftExpr->GetDeducedType())
        };

        auto &builder{parsing::CompilerState::GetInstance().GetBuilder()};

        if (astShiftExpr->GetDeducedType().IsInteger()) {
            switch (astShiftExpr->GetOperator()) {
                case parsing::ShiftOperator::Left:
                    m_Value =
                            builder.CreateShl(lhsValue, rhsValue, "shl_result");
                    break;
                case parsing::ShiftOperator::Right:
                    m_Value = builder.CreateAShr(
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

        auto *llvmType{type.GetLLVMType()};

        m_Value = llvm::ConstantFP::get(llvmType, value);
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstCastExpression const *astCastExpr
    ) {
        auto const *expression{astCastExpr->GetExpression()};
        expression->AcceptOnExpression(this);
        auto       *value{GetValue()};
        auto const &type{astCastExpr->GetTypeName().GetType()};

        m_Value = CastValue(value, expression->GetDeducedType(), type);
    }

    [[nodiscard]]
    llvm::Value *RelationalCmpFloat(
            llvm::Value *lhs, llvm::Value *rhs, parsing::RelationalOperator op
    ) {
        auto &builder{parsing::CompilerState::GetInstance().GetBuilder()};

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
    }

    [[nodiscard]]
    llvm::Value *RelationalCmpSignedInt(
            llvm::Value *lhs, llvm::Value *rhs, parsing::RelationalOperator op
    ) {
        auto &builder{parsing::CompilerState::GetInstance().GetBuilder()};

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
    }

    [[nodiscard]]
    llvm::Value *RelationalCmpUnsignedInt(
            llvm::Value *lhs, llvm::Value *rhs, parsing::RelationalOperator op
    ) {
        auto &builder{parsing::CompilerState::GetInstance().GetBuilder()};

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
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstRelationalExpression const *astRelationalExpr
    ) {

        auto const type{astRelationalExpr->GetUsualArithmeticConversionType()};
        auto [lhsValue, rhsValue]{PrepareOperands(astRelationalExpr, type)};

        if (type.IsFloating()) {
            m_Value = RelationalCmpFloat(
                    lhsValue, rhsValue, astRelationalExpr->GetOperator()
            );
        } else if (type.IsInteger()) {
            auto const intType{std::get<parsing::types::IntegerType>(
                    astRelationalExpr->GetDeducedType().GetType()
            )};

            if (intType.IsSigned()) {
                m_Value = RelationalCmpSignedInt(
                        lhsValue, rhsValue, astRelationalExpr->GetOperator()
                );
            } else {
                m_Value = RelationalCmpUnsignedInt(
                        lhsValue, rhsValue, astRelationalExpr->GetOperator()
                );
            }
        } else {
            parsing::CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<diagnostics::TodoError>(
                            astRelationalExpr->m_Span.m_Source,
                            astRelationalExpr->m_Span.m_Span
                    );
        }

        auto       &builder{parsing::CompilerState::GetInstance().GetBuilder()};
        auto *const intType{parsing::types::IntegerType::GetLLVMType(
                parsing::types::StandardIntegerType::Int
        )};

        m_Value =
                builder.CreateZExt(m_Value, intType, "relational_result_zext");
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstEqualityExpression const *astEqualityExpression
    ) {
        auto const type{astEqualityExpression->GetUsualArithmeticConversionType(
        )};
        auto [lhsValue, rhsValue]{PrepareOperands(astEqualityExpression, type)};

        auto &builder{parsing::CompilerState::GetInstance().GetBuilder()};
        if (type.IsArithmetic()) {
            if (type.IsFloating()) {
                switch (astEqualityExpression->GetOperator()) {
                    case parsing::EqualityOperator::Equal:
                        m_Value = builder.CreateFCmpOEQ(
                                lhsValue, rhsValue, "fp_eq_result"
                        );
                        break;
                    case parsing::EqualityOperator::NotEqual:
                        m_Value = builder.CreateFCmpONE(
                                lhsValue, rhsValue, "fp_ne_result"
                        );
                        break;
                }
            } else if (type.IsInteger()) {
                switch (astEqualityExpression->GetOperator()) {
                    case parsing::EqualityOperator::Equal:
                        m_Value = builder.CreateICmpEQ(
                                lhsValue, rhsValue, "int_eq_result"
                        );
                        break;
                    case parsing::EqualityOperator::NotEqual:
                        m_Value = builder.CreateICmpNE(
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


        auto *const intType{parsing::types::IntegerType::GetLLVMType(
                parsing::types::StandardIntegerType::Int
        )};

        m_Value = builder.CreateZExt(m_Value, intType, "equality_result_zext");
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstBitwiseAndExpression const *astBitwiseAndExpr
    ) {
        auto const type{astBitwiseAndExpr->GetDeducedType()};
        auto [lhsValue, rhsValue]{PrepareOperands(astBitwiseAndExpr, type)};

        auto &builder{parsing::CompilerState::GetInstance().GetBuilder()};
        m_Value = builder.CreateAnd(lhsValue, rhsValue, "and_result");
    }

    void ExpressionCodegenVisitor::Visit(
            parsing::AstBitwiseXorExpression const *astBitwiseXorExpr
    ) {
        auto const type{astBitwiseXorExpr->GetDeducedType()};
        auto [lhsValue, rhsValue]{PrepareOperands(astBitwiseXorExpr, type)};

        auto &builder{parsing::CompilerState::GetInstance().GetBuilder()};
        m_Value = builder.CreateXor(lhsValue, rhsValue, "xor_result");
    }

    llvm::Value *ExpressionCodegenVisitor::GetValue() noexcept {
        auto const copy{m_Value};
        m_Value = nullptr;

        return copy;
    }
}// namespace jcc::visitors
