#include "expression_codegen.h"

#include "parsing/additive_expression.h"
#include "parsing/cast_expression.hpp"
#include "parsing/multiplicative_expression.h"
#include "parsing/numeric_constant.h"
#include "parsing/shift_expression.h"

namespace jcc::visitors {
    std::pair<llvm::Value *, llvm::Value *>
    ExpressionCodegenVisitor::PrepareOperands(
            parsing::AstBinaryExpression const *astExpr
    ) {
        auto const *lhs{astExpr->GetLhs()};
        auto const *rhs{astExpr->GetRhs()};

        lhs->AcceptOnExpression(this);
        auto *lhsValue{GetValue()};

        if (lhs->GetDeducedType() != astExpr->GetDeducedType()) {
            lhsValue = CastValue(
                    lhsValue, lhs->GetDeducedType(), astExpr->GetDeducedType()
            );
        }

        rhs->AcceptOnExpression(this);
        auto *rhsValue{GetValue()};

        if (rhs->GetDeducedType() != astExpr->GetDeducedType()) {
            rhsValue = CastValue(
                    rhsValue, rhs->GetDeducedType(), astExpr->GetDeducedType()
            );
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
        auto [lhsValue, rhsValue]{PrepareOperands(astMultiplicativeExpr)};

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
        auto [lhsValue, rhsValue]{PrepareOperands(astAdditiveExpr)};

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
        auto [lhsValue, rhsValue]{PrepareOperands(astShiftExpr)};

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

    llvm::Value *ExpressionCodegenVisitor::GetValue() noexcept {
        auto const copy{m_Value};
        m_Value = nullptr;

        return copy;
    }
}// namespace jcc::visitors
