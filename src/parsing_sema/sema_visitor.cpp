#include "sema_visitor.h"

#include "additive_expression.h"
#include "diagnostics/variants/sema/modulo_with_non_int.hpp"
#include "diagnostics/variants/sema/mult_non_arithmetic.hpp"
#include "diagnostics/variants/sema/shift_operand_non_int.hpp"
#include "diagnostics/variants/todo.hpp"
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

        auto const rhs{astMultiplicativeExpr->GetRhs()};
        rhs->Accept(this);

        auto const rhsType{rhs->GetType()};

        // Because we want to check both children, we can't just return after checking one of them.
        bool canContinue{true};
        if (!lhsType.IsArithmetic()) {
            compilerState.EmplaceDiagnostic<diagnostics::MultNonArithmetic>(
                    lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                    rhs->m_Span.m_Span, astMultiplicativeExpr->GetOpSpan(),
                    lhsType, rhsType
            );
            canContinue = false;
        }

        if (!rhsType.IsArithmetic()) {
            compilerState.EmplaceDiagnostic<diagnostics::MultNonArithmetic>(
                    lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                    rhs->m_Span.m_Span, astMultiplicativeExpr->GetOpSpan(),
                    lhsType, rhsType
            );
            canContinue = false;
        }

        if (!canContinue)
            return;

        if (astMultiplicativeExpr->GetOperator() ==
            MultiplicativeOperator::Modulo) {
            if (lhsType.IsInteger() && rhsType.IsInteger())
                return;// OK

            compilerState.EmplaceDiagnostic<diagnostics::ModuloWithNonInt>(
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
            compilerState.EmplaceDiagnostic<diagnostics::TodoError>(
                    astAdditiveExpr->m_Span.m_Source,
                    astAdditiveExpr->m_Span.m_Span
            );
            return;
        }

        // Subtraction
        if (lhsType.IsArithmetic() && rhsType.IsArithmetic())
            return;// OK

        // TODO: Either both operands shall have integer type or the left operand shall have pointer type and the right operand shall have integer type.
        compilerState.EmplaceDiagnostic<diagnostics::TodoError>(
                astAdditiveExpr->m_Span.m_Source, astAdditiveExpr->m_Span.m_Span
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

        compilerState.EmplaceDiagnostic<diagnostics::ShiftOperandNonInt>(
                astShiftExpr->m_Span.m_Source, lhs->m_Span.m_Span,
                rhs->m_Span.m_Span, astShiftExpr->GetOpSpan(), lhsType, rhsType
        );
    }

    void SemaVisitor::Visit(AstFloatingConstant const *) {
        // Nothing to do here
    }
}// namespace jcc::parsing_sema
