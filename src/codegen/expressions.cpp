#include "expressions.h"

#include "parsing_sema/multiplicative_expression.h"
#include "parsing_sema/numeric_constant.h"

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
            lhsValue = CastValue(lhsValue, astMultiplicativeExpr->GetType());
        }

        rhs->Accept(this);
        auto *rhsValue{GetValue()};

        if (rhs->GetType() != astMultiplicativeExpr->GetType()) {
            rhsValue = CastValue(rhsValue, astMultiplicativeExpr->GetType());
        }


        auto &builder{parsing_sema::CompilerState::GetInstance().GetBuilder()};

        switch (astMultiplicativeExpr->GetOperator()) {
            case parsing_sema::MultiplicativeOperator::Multiplication:
                m_Value = builder.CreateMul(lhsValue, rhsValue, "mult_result");
                break;
            case parsing_sema::MultiplicativeOperator::Division:
                m_Value = builder.CreateSDiv(lhsValue, rhsValue, "div_result");
                break;
            case parsing_sema::MultiplicativeOperator::Modulo:
                m_Value = builder.CreateSRem(lhsValue, rhsValue, "mod_result");
                break;
        }
    }

    llvm::Value *ExpressionCodegenVisitor::GetValue() const noexcept {
        auto const copy{m_Value};
        m_Value = nullptr;

        return copy;
    }
}// namespace jcc::codegen
