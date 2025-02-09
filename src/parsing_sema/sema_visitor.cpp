#include "sema_visitor.h"

#include "additive_expression.h"
#include "diagnostics/variants/sema_modulo_int.h"
#include "diagnostics/variants/sema_mult_arithmetic.h"
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
        lhs->Accept(this);

        auto const lhsType{lhs->GetType()};

        if (!lhsType.IsArithmetic()) {
            compilerState.EmplaceDiagnosis(
                    lhs->m_Span, Diagnosis::Class::Error,
                    Diagnosis::Kind::SEMA_MultOperandMustBeArithmetic
            );
        }

        auto const rhs{astMultiplicativeExpr->GetRhs()};
        rhs->Accept(this);

        auto const rhsType{rhs->GetType()};

        if (!rhsType.IsArithmetic()) {
            compilerState.EmplaceDiagnostic<diagnostics::SemaMultArithmetic>(
                    lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                    rhs->m_Span.m_Span, astMultiplicativeExpr->GetOpSpan(),
                    lhsType, rhsType
            );
        }

        if (astMultiplicativeExpr->GetOperator() ==
            MultiplicativeOperator::Modulo) {
            if (lhsType.IsInteger() && rhsType.IsInteger())
                return;// OK

            compilerState.EmplaceDiagnostic<diagnostics::SemaModuloInt>(
                    lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                    rhs->m_Span.m_Span, astMultiplicativeExpr->GetOpSpan(),
                    lhsType, rhsType
            );
        }
    }

    void SemaVisitor::Visit(AstAdditiveExpression const *astAdditiveExpr) {
        auto &compilerState{CompilerState::GetInstance()};

        auto const lhs{astAdditiveExpr->GetLhs()};
        lhs->Accept(this);

        auto const &lhsType{lhs->GetType()};

        auto const rhs{astAdditiveExpr->GetRhs()};
        rhs->Accept(this);

        auto const &rhsType{rhs->GetType()};

        if (astAdditiveExpr->GetOperator() == AdditiveOperator::Addition) {
            if (lhsType.IsArithmetic() && rhsType.IsArithmetic())
                return;// OK

            // TODO: If one of the operands is a pointer, the other must be an integer.
            compilerState.EmplaceDiagnosis(
                    astAdditiveExpr->m_Span, Diagnosis::Class::Error,
                    Diagnosis::Kind::SEMA_IncompatibleOperands
            );
            return;
        }

        // Subtraction
        if (lhsType.IsArithmetic() && rhsType.IsArithmetic())
            return;// OK

        // TODO: Either both operands shall have integer type or the left operand shall have pointer type and the right operand shall have integer type.
        compilerState.EmplaceDiagnosis(
                astAdditiveExpr->m_Span, Diagnosis::Class::Error,
                Diagnosis::Kind::SEMA_IncompatibleOperands
        );
    }

    void SemaVisitor::Visit(AstShiftExpression const *astShiftExpr) {
        auto &compilerState{CompilerState::GetInstance()};

        auto const lhs{astShiftExpr->GetLhs()};
        lhs->Accept(this);

        auto const &lhsType{lhs->GetType()};

        auto const rhs{astShiftExpr->GetRhs()};
        rhs->Accept(this);

        auto const &rhsType{rhs->GetType()};

        if (lhsType.IsInteger() && rhsType.IsInteger())
            return;

        compilerState.EmplaceDiagnosis(
                astShiftExpr->m_Span, Diagnosis::Class::Error,
                Diagnosis::Kind::SEMA_ShiftOperandNotInteger
        );
    }

    void SemaVisitor::Visit(AstFloatingConstant const *) {
        // Nothing to do here
    }
}// namespace jcc::parsing_sema
