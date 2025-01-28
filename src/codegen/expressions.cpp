#include "expressions.h"

#include "parsing_sema/additive_expression.h"
#include "parsing_sema/multiplicative_expression.h"
#include "parsing_sema/numeric_constant.h"
#include "parsing_sema/shift_expression.h"

namespace jcc::codegen {
    std::pair<llvm::Value *, llvm::Value *>
    ExpressionCodegenVisitor::PrepareOperands(
            parsing_sema::AstBinaryExpression const *astExpr
    ) {
        auto const *lhs{astExpr->GetLhs()};
        auto const *rhs{astExpr->GetRhs()};

        lhs->Accept(this);
        auto *lhsValue{GetValue()};

        if (lhs->GetType() != astExpr->GetType()) {
            lhsValue = CastValue(lhsValue, lhs->GetType(), astExpr->GetType());
        }

        rhs->Accept(this);
        auto *rhsValue{GetValue()};

        if (rhs->GetType() != astExpr->GetType()) {
            rhsValue = CastValue(rhsValue, rhs->GetType(), astExpr->GetType());
        }

        return {lhsValue, rhsValue};
    }

    void ExpressionCodegenVisitor::Visit(
            parsing_sema::AstIntegerConstant const *astIntConst
    ) {
        auto const value{astIntConst->GetValue()};

        auto const type{std::get<parsing_sema::types::IntegerType>(
                astIntConst->GetType().GetType()
        )};

        auto *llvmType{type.GetLLVMType()};

        m_Value = llvm::ConstantInt::get(llvmType, value, type.IsSigned());
    }

    void ExpressionCodegenVisitor::Visit(
            parsing_sema::AstMultiplicativeExpression const
                    *astMultiplicativeExpr
    ) {
        auto [lhsValue, rhsValue]{PrepareOperands(astMultiplicativeExpr)};

        auto &builder{parsing_sema::CompilerState::GetInstance().GetBuilder()};
        auto const &type{astMultiplicativeExpr->GetType()};

        if (type.IsInteger()) {
            auto const intType{std::get<parsing_sema::types::IntegerType>(
                    astMultiplicativeExpr->GetType().GetType()
            )};

            switch (astMultiplicativeExpr->GetOperator()) {
                case parsing_sema::MultiplicativeOperator::Multiplication:
                    m_Value = builder.CreateMul(
                            lhsValue, rhsValue, "mult_result"
                    );
                    break;
                case parsing_sema::MultiplicativeOperator::Division:
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
                case parsing_sema::MultiplicativeOperator::Modulo:
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
                case parsing_sema::MultiplicativeOperator::Multiplication:
                    m_Value = builder.CreateFMul(
                            lhsValue, rhsValue, "fmult_result"
                    );
                    return;
                case parsing_sema::MultiplicativeOperator::Division:
                    m_Value = builder.CreateFDiv(
                            lhsValue, rhsValue, "fdiv_result"
                    );
                    return;
                case parsing_sema::MultiplicativeOperator::Modulo:
                    assert(false &&
                           "Floating point modulo not supported, and should "
                           "have been caught by the semantic analysis.");
                    break;
            }
        }

        throw std::runtime_error{"Unsupported multiplicative expression type"};
    }

    void ExpressionCodegenVisitor::Visit(
            parsing_sema::AstAdditiveExpression const *astAdditiveExpr
    ) {
        auto [lhsValue, rhsValue]{PrepareOperands(astAdditiveExpr)};

        auto &builder{parsing_sema::CompilerState::GetInstance().GetBuilder()};

        if (astAdditiveExpr->GetType().IsInteger()) {
            switch (astAdditiveExpr->GetOperator()) {
                case parsing_sema::AdditiveOperator::Addition:
                    m_Value =
                            builder.CreateAdd(lhsValue, rhsValue, "add_result");
                    break;
                case parsing_sema::AdditiveOperator::Subtraction:
                    m_Value =
                            builder.CreateSub(lhsValue, rhsValue, "sub_result");
                    break;
            }
            return;
        }

        if (astAdditiveExpr->GetType().IsFloating()) {
            switch (astAdditiveExpr->GetOperator()) {
                case parsing_sema::AdditiveOperator::Addition:
                    m_Value = builder.CreateFAdd(
                            lhsValue, rhsValue, "fadd_result"
                    );
                    break;
                case parsing_sema::AdditiveOperator::Subtraction:
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
            parsing_sema::AstShiftExpression const *astShiftExpr
    ) {
        auto [lhsValue, rhsValue]{PrepareOperands(astShiftExpr)};

        auto &builder{parsing_sema::CompilerState::GetInstance().GetBuilder()};

        if (astShiftExpr->GetType().IsInteger()) {
            switch (astShiftExpr->GetOperator()) {
                case parsing_sema::ShiftOperator::Left:
                    m_Value =
                            builder.CreateShl(lhsValue, rhsValue, "shl_result");
                    break;
                case parsing_sema::ShiftOperator::Right:
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
            parsing_sema::AstFloatingConstant const *astFloatingConst
    ) {
        auto const value{astFloatingConst->GetValue()};

        auto const type{std::get<parsing_sema::types::FloatingType>(
                astFloatingConst->GetType().GetType()
        )};

        auto *llvmType{type.GetLLVMType()};

        m_Value = llvm::ConstantFP::get(llvmType, value);
    }

    llvm::Value *ExpressionCodegenVisitor::GetValue() noexcept {
        auto const copy{m_Value};
        m_Value = nullptr;

        return copy;
    }
}// namespace jcc::codegen
