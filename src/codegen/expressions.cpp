#include "expressions.h"

#include "parsing_sema/additive_expression.h"
#include "parsing_sema/multiplicative_expression.h"
#include "parsing_sema/numeric_constant.h"
#include "parsing_sema/shift_expression.h"

namespace jcc::codegen {
    void ExpressionCodegenVisitor::Visit(
            parsing_sema::AstIntegerConstant const *astIntConst
    ) {
        auto const value{astIntConst->GetValue()};

        auto const type{std::get<parsing_sema::types::IntegerType>(
                astIntConst->GetType().GetInnerType()
        )};

        auto *llvmType{type.GetLLVMType()};

        m_Value = llvm::ConstantInt::get(llvmType, value, type.IsSigned());
    }

    void ExpressionCodegenVisitor::Visit(
            parsing_sema::AstMultiplicativeExpression const
                    *astMultiplicativeExpr
    ) {
        auto const *lhs{astMultiplicativeExpr->GetLhs()};
        auto const *rhs{astMultiplicativeExpr->GetRhs()};

        lhs->Accept(this);
        auto *lhsValue{GetValue()};

        if (lhs->GetType() != astMultiplicativeExpr->GetType()) {
            lhsValue = CastValue(
                    lhsValue, lhs->GetType(), astMultiplicativeExpr->GetType()
            );
        }

        rhs->Accept(this);
        auto *rhsValue{GetValue()};

        if (rhs->GetType() != astMultiplicativeExpr->GetType()) {
            rhsValue = CastValue(
                    rhsValue, rhs->GetType(), astMultiplicativeExpr->GetType()
            );
        }


        auto &builder{parsing_sema::CompilerState::GetInstance().GetBuilder()};

        if (astMultiplicativeExpr->GetType().IsInteger()) {
            auto const type{std::get<parsing_sema::types::IntegerType>(
                    astMultiplicativeExpr->GetType().GetInnerType()
            )};

            switch (astMultiplicativeExpr->GetOperator()) {
                case parsing_sema::MultiplicativeOperator::Multiplication:
                    m_Value = builder.CreateMul(
                            lhsValue, rhsValue, "mult_result"
                    );
                    break;
                case parsing_sema::MultiplicativeOperator::Division:
                    if (type.IsSigned()) {
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
                    if (type.IsSigned()) {
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

        throw std::runtime_error{"Unsupported multiplicative expression type"};
    }

    void ExpressionCodegenVisitor::Visit(
            parsing_sema::AstAdditiveExpression const *astAdditiveExpr
    ) {
        auto const *lhs{astAdditiveExpr->GetLhs()};
        auto const *rhs{astAdditiveExpr->GetRhs()};

        lhs->Accept(this);
        auto *lhsValue{GetValue()};

        if (lhs->GetType() != astAdditiveExpr->GetType()) {
            lhsValue = CastValue(
                    lhsValue, lhs->GetType(), astAdditiveExpr->GetType()
            );
        }

        rhs->Accept(this);
        auto *rhsValue{GetValue()};

        if (rhs->GetType() != astAdditiveExpr->GetType()) {
            rhsValue = CastValue(
                    rhsValue, rhs->GetType(), astAdditiveExpr->GetType()
            );
        }

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

        throw std::runtime_error{"Unsupported additive expression type"};
    }

    void ExpressionCodegenVisitor::Visit(
            parsing_sema::AstShiftExpression const *astShiftExpr
    ) {
        auto const *lhs{astShiftExpr->GetLhs()};
        auto const *rhs{astShiftExpr->GetRhs()};

        lhs->Accept(this);
        auto *lhsValue{GetValue()};

        if (lhs->GetType() != astShiftExpr->GetType()) {
            lhsValue = CastValue(
                    lhsValue, lhs->GetType(), astShiftExpr->GetType()
            );
        }

        rhs->Accept(this);
        auto *rhsValue{GetValue()};

        if (rhs->GetType() != astShiftExpr->GetType()) {
            rhsValue = CastValue(
                    rhsValue, rhs->GetType(), astShiftExpr->GetType()
            );
        }

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

    llvm::Value *ExpressionCodegenVisitor::GetValue() const noexcept {
        auto const copy{m_Value};
        m_Value = nullptr;

        return copy;
    }
}// namespace jcc::codegen
