#include "sema.h"

#include <string>
#include <utility>

#include "diagnostics/variants/sema/binary_operands_wrong_types.hpp"
#include "diagnostics/variants/sema/invalid_specifier_qualifier_list.hpp"
#include "diagnostics/variants/todo.hpp"
#include "misc/Span.h"
#include "parsing/additive_expression.h"
#include "parsing/bitwise_and_expression.hpp"
#include "parsing/bitwise_or_expression.hpp"
#include "parsing/bitwise_xor_expression.hpp"
#include "parsing/cast_expression.hpp"
#include "parsing/equality_expression.hpp"
#include "parsing/logical_and_expression.hpp"
#include "parsing/multiplicative_expression.h"
#include "parsing/parser.h"
#include "parsing/relational_expression.hpp"
#include "parsing/shift_expression.h"
#include "parsing/type_name.h"
#include "parsing/types.h"
#include "parsing/types/type.h"

namespace jcc {
    namespace parsing {
        class AstFloatingConstant;
        class AstIntegerConstant;
    }// namespace parsing
}// namespace jcc

namespace jcc::visitors {
    using namespace parsing;

    void
    SemaVisitor::SemaBitwiseExpression(AstBinaryExpression const *astBinaryExpr
    ) {
        auto const lhs{astBinaryExpr->GetLhs()};
        lhs->AcceptOnExpression(this);

        auto const &lhsType{lhs->GetDeducedType()};

        auto const rhs{astBinaryExpr->GetRhs()};
        rhs->AcceptOnExpression(this);

        auto const &rhsType{rhs->GetDeducedType()};

        if (lhsType.IsInteger() && rhsType.IsInteger())
            return;

        CompilerState::GetInstance()
                .EmplaceDiagnostic<diagnostics::BinaryOperandsWrongTypes>(
                        lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                        rhs->m_Span.m_Span, astBinaryExpr->GetOpSpan(),
                        "The operands of a bitwise expression must "
                        "both be of integer types.",
                        lhsType, rhsType
                );
    }

    void SemaVisitor::Visit(AstIntegerConstant const *) {
        // Nothing to do here
    }

    void
    SemaVisitor::Visit(AstMultiplicativeExpression const *astMultiplicativeExpr
    ) {
        // TODO: If either operand has decimal floating type, the other operand shall not have standard floating type, complex type, or imaginary type.

        auto &compilerState{CompilerState::GetInstance()};

        auto const lhs{astMultiplicativeExpr->GetLhs()};
        lhs->AcceptOnExpression(this);

        auto const lhsType{lhs->GetDeducedType()};

        auto const rhs{astMultiplicativeExpr->GetRhs()};
        rhs->AcceptOnExpression(this);

        auto const rhsType{rhs->GetDeducedType()};

        std::string errorMessage{
                "The operands of a multiplicative expression must "
                "both be of arithmetic types."
        };

        // Because we want to check both children, we can't just return after checking one of them.
        bool canContinue{true};
        if (!lhsType.IsArithmetic()) {
            compilerState
                    .EmplaceDiagnostic<diagnostics::BinaryOperandsWrongTypes>(
                            lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                            rhs->m_Span.m_Span,
                            astMultiplicativeExpr->GetOpSpan(), errorMessage,
                            lhsType, rhsType
                    );
            canContinue = false;
        }

        if (!rhsType.IsArithmetic()) {
            compilerState
                    .EmplaceDiagnostic<diagnostics::BinaryOperandsWrongTypes>(
                            lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                            rhs->m_Span.m_Span,
                            astMultiplicativeExpr->GetOpSpan(),
                            std::move(errorMessage), lhsType, rhsType
                    );
            canContinue = false;
        }

        if (!canContinue)
            return;

        if (astMultiplicativeExpr->GetOperator() ==
            MultiplicativeOperator::Modulo) {
            if (lhsType.IsInteger() && rhsType.IsInteger())
                return;// OK

            compilerState
                    .EmplaceDiagnostic<diagnostics::BinaryOperandsWrongTypes>(
                            lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                            rhs->m_Span.m_Span,
                            astMultiplicativeExpr->GetOpSpan(),
                            "Both operands must be of an integer type when "
                            "using the modulo operator.",
                            lhsType, rhsType
                    );
        }
    }

    void SemaVisitor::Visit(AstAdditiveExpression const *astAdditiveExpr) {
        auto &compilerState{CompilerState::GetInstance()};

        auto const lhs{astAdditiveExpr->GetLhs()};
        lhs->AcceptOnExpression(this);

        auto const &lhsType{lhs->GetDeducedType()};

        auto const rhs{astAdditiveExpr->GetRhs()};
        rhs->AcceptOnExpression(this);

        auto const &rhsType{rhs->GetDeducedType()};

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
        lhs->AcceptOnExpression(this);

        auto const &lhsType{lhs->GetDeducedType()};

        auto const rhs{astShiftExpr->GetRhs()};
        rhs->AcceptOnExpression(this);

        auto const &rhsType{rhs->GetDeducedType()};

        if (lhsType.IsInteger() && rhsType.IsInteger())
            return;

        compilerState.EmplaceDiagnostic<diagnostics::BinaryOperandsWrongTypes>(
                astShiftExpr->m_Span.m_Source, lhs->m_Span.m_Span,
                rhs->m_Span.m_Span, astShiftExpr->GetOpSpan(),
                "The operands of a shift expression must "
                "both be of integer types.",
                lhsType, rhsType
        );
    }

    void SemaVisitor::Visit(AstFloatingConstant const *) {
        // Nothing to do here
    }

    void SemaVisitor::Visit(AstCastExpression const *astCastExpr) {
        astCastExpr->GetTypeName().AcceptOnNode(this);
        astCastExpr->GetExpression()->AcceptOnExpression(this);
    }

    void SemaVisitor::Visit(SpecifierQualifierList const *specifierQualifierList
    ) {
        auto const typeSpecifierQualifiers{
                specifierQualifierList->GetTypeSpecifierQualifier()
        };

        if (!typeSpecifierQualifiers.m_Specifiers.IsValid()) {
            CompilerState::GetInstance()
                    .EmplaceDiagnostic<
                            diagnostics::InvalidSpecifierQualifierList>(
                            specifierQualifierList->m_Span.m_Source,
                            specifierQualifierList->m_Span.m_Span
                    );
        }
        // TODO: Check the qualifiers
    }

    void SemaVisitor::Visit(AstTypeName const *astTypeName) {
        astTypeName->GetSpecifierQualifierList().AcceptOnNode(this);
    }

    void SemaVisitor::Visit(AstRelationalExpression const *astRelationalExpr) {
        auto const lhs{astRelationalExpr->GetLhs()};
        lhs->AcceptOnExpression(this);

        auto const &lhsType{lhs->GetDeducedType()};

        auto const rhs{astRelationalExpr->GetRhs()};
        rhs->AcceptOnExpression(this);

        auto const &rhsType{rhs->GetDeducedType()};

        if (lhsType.IsReal() && rhsType.IsReal())
            return;

        // TODO: Handle the case of both operands being pointers to qualified or unqualified versions of compatible object types.

        CompilerState::GetInstance()
                .EmplaceDiagnostic<diagnostics::BinaryOperandsWrongTypes>(
                        lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                        rhs->m_Span.m_Span, astRelationalExpr->GetOpSpan(),
                        "Both operands must be a real type or pointers to "
                        "compatible types.",
                        lhsType, rhsType
                );
    }

    void SemaVisitor::Visit(AstEqualityExpression const *astEqualityExpression
    ) {
        auto const lhs{astEqualityExpression->GetLhs()};
        lhs->AcceptOnExpression(this);

        auto const &lhsType{lhs->GetDeducedType()};

        auto const rhs{astEqualityExpression->GetRhs()};
        rhs->AcceptOnExpression(this);

        auto const &rhsType{rhs->GetDeducedType()};

        if (lhsType.IsArithmetic() && rhsType.IsArithmetic())
            return;

        // TODO: handle the other cases

        CompilerState::GetInstance()
                .EmplaceDiagnostic<diagnostics::BinaryOperandsWrongTypes>(
                        lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                        rhs->m_Span.m_Span, astEqualityExpression->GetOpSpan(),
                        "Both operands must be of arithmetic types or pointers "
                        "to compatible types.",
                        lhsType, rhsType
                );
    }

    void SemaVisitor::Visit(AstBitwiseAndExpression const *astBitwiseAndExpr) {
        SemaBitwiseExpression(astBitwiseAndExpr);
    }

    void SemaVisitor::Visit(AstBitwiseXorExpression const *astBitwiseXorExpr) {
        SemaBitwiseExpression(astBitwiseXorExpr);
    }

    void SemaVisitor::Visit(AstBitwiseOrExpression const *astBitwiseOrExpr) {
        SemaBitwiseExpression(astBitwiseOrExpr);
    }

    void SemaVisitor::Visit(AstLogicalAndExpression const *astLogicalAndExpr) {
        auto const lhs{astLogicalAndExpr->GetLhs()};
        lhs->AcceptOnExpression(this);

        auto const &lhsType{lhs->GetDeducedType()};

        auto const rhs{astLogicalAndExpr->GetRhs()};
        rhs->AcceptOnExpression(this);

        auto const &rhsType{rhs->GetDeducedType()};

        if (lhsType.IsScalar() && rhsType.IsScalar())
            return;

        CompilerState::GetInstance()
                .EmplaceDiagnostic<diagnostics::BinaryOperandsWrongTypes>(
                        lhs->m_Span.m_Source, lhs->m_Span.m_Span,
                        rhs->m_Span.m_Span, astLogicalAndExpr->GetOpSpan(),
                        "Each operand must be a scalar type.", lhsType, rhsType
                );
    }
}// namespace jcc::visitors
