#include "sema_visitor.h"

#include "additive_expression.h"
#include "multiplicative_expression.h"
#include "shift_expression.h"

namespace jcc::parsing_sema {
    void SemaVisitor::Visit(AstIntegerConstant const *) {
        // Nothing to do here
    }

    void SemaVisitor::Visit(
            AstMultiplicativeExpression const *astMultiplicativeExpr
    ) {
        // TODO: If either operand has decimal floating type, the other operand shall not have standard floating type, complex type, or imaginary type.

        auto &compilerState{CompilerState::GetInstance()};

        auto const lhs{astMultiplicativeExpr->GetLhs()};

        if (auto const lhsType{lhs->GetType()}; !lhsType.IsArithmetic()) {
            compilerState.EmplaceDiagnosis(
                    lhs->GetSpan(), Diagnosis::Class::Error,
                    Diagnosis::Kind::SEMA_MultOperandMustBeArithmetic
            );
        }

        auto const rhs{astMultiplicativeExpr->GetRhs()};

        if (auto const rhsType{rhs->GetType()}; !rhsType.IsArithmetic()) {
            compilerState.EmplaceDiagnosis(
                    rhs->GetSpan(), Diagnosis::Class::Error,
                    Diagnosis::Kind::SEMA_MultOperandMustBeArithmetic
            );
        }
    }

    void SemaVisitor::Visit(AstAdditiveExpression const *astAdditiveExpr) {
        auto &compilerState{CompilerState::GetInstance()};

        auto const  lhs{astAdditiveExpr->GetLhs()};
        auto const &lhsType{lhs->GetType()};
        auto const  rhs{astAdditiveExpr->GetRhs()};
        auto const &rhsType{rhs->GetType()};

        if (astAdditiveExpr->GetOperator() == AdditiveOperator::Addition) {
            if (lhsType.IsArithmetic() && rhsType.IsArithmetic())
                return;// OK

            // TODO: If one of the operands is a pointer, the other must be an integer.
            compilerState.EmplaceDiagnosis(
                    astAdditiveExpr->GetSpan(), Diagnosis::Class::Error,
                    Diagnosis::Kind::SEMA_IncompatibleOperands
            );
            return;
        }

        // Subtraction
        if (lhsType.IsArithmetic() && rhsType.IsArithmetic())
            return;// OK

        // TODO: Either both operands shall have integer type or the left operand shall have pointer type and the right operand shall have integer type.
        compilerState.EmplaceDiagnosis(
                astAdditiveExpr->GetSpan(), Diagnosis::Class::Error,
                Diagnosis::Kind::SEMA_IncompatibleOperands
        );
    }

    void SemaVisitor::Visit(AstShiftExpression const *astShiftExpr) {
        auto &compilerState{CompilerState::GetInstance()};

        auto const  lhs{astShiftExpr->GetLhs()};
        auto const &lhsType{lhs->GetType()};
        auto const  rhs{astShiftExpr->GetRhs()};
        auto const &rhsType{rhs->GetType()};

        if (lhsType.IsInteger() && rhsType.IsInteger())
            return;

        compilerState.EmplaceDiagnosis(
                astShiftExpr->GetSpan(), Diagnosis::Class::Error,
                Diagnosis::Kind::SEMA_IncompatibleOperands
        );
    }
}// namespace jcc::parsing_sema
