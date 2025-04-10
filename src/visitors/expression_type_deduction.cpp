#include "expression_type_deduction.hpp"

#include <parsing/cast_expression.hpp>

#include "diagnostics/variants/sema/invalid_specifier_qualifier_list.hpp"
#include "parsing/additive_expression.h"
#include "parsing/bitwise_and_expression.hpp"
#include "parsing/bitwise_or_expression.hpp"
#include "parsing/bitwise_xor_expression.hpp"
#include "parsing/equality_expression.hpp"
#include "parsing/logical_and_expression.hpp"
#include "parsing/multiplicative_expression.h"
#include "parsing/numeric_constant.h"
#include "parsing/relational_expression.hpp"
#include "parsing/shift_expression.h"

namespace jcc::visitors {
    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstIntegerConstant const *astIntConst
    ) {
        parsing::types::ValueType const type{astIntConst->GetNumericType()};

        astIntConst->SetDeducedType(type);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstFloatingConstant const *astFloatingConst
    ) {
        parsing::types::ValueType const type{astFloatingConst->GetNumericType()
        };

        astFloatingConst->SetDeducedType(type);
    }

    void ExpressionTypeDeductionVisitor::BinaryExprTypeDeduction(
            parsing::AstBinaryExpression const *astBinaryExpr
    ) {
        auto const *lhs{astBinaryExpr->GetLhs()};
        lhs->AcceptOnExpression(this);
        auto const *rhs{astBinaryExpr->GetRhs()};
        rhs->AcceptOnExpression(this);

        auto const type{UsualArithmeticConversions(
                lhs->GetDeducedType(), rhs->GetDeducedType()
        )};

        astBinaryExpr->SetDeducedType(type);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstAdditiveExpression const *astAdditiveExpr
    ) {
        BinaryExprTypeDeduction(astAdditiveExpr);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstMultiplicativeExpression const *astMultiplicativeExpr
    ) {
        BinaryExprTypeDeduction(astMultiplicativeExpr);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstShiftExpression const *astShiftExpr
    ) {
        BinaryExprTypeDeduction(astShiftExpr);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstCastExpression const *astCastExpr
    ) {
        if (!astCastExpr->GetTypeName().GetSpecifierQualifierList().IsValid()) {
            auto const &span{astCastExpr->GetTypeName()
                                     .GetSpecifierQualifierList()
                                     .m_Span};

            parsing::CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<
                            diagnostics::InvalidSpecifierQualifierList>(
                            span.m_Source, span.m_Span
                    );
        }
        astCastExpr->GetExpression()->AcceptOnExpression(this);

        astCastExpr->SetDeducedType(astCastExpr->GetTypeName().GetType());
    }

    void ExpressionTypeDeductionVisitor::CmpTypeDeduction(
            parsing::AstBooleanBinaryExpression const
                    *astBooleanBinaryExpression
    ) {
        using namespace parsing::types;

        IntegerType const intType{
                StandardIntegerType::Int, IntegerType::Signedness::Unspecified
        };

        astBooleanBinaryExpression->SetDeducedType(ValueType{intType});

        auto const *lhs{astBooleanBinaryExpression->GetLhs()};
        lhs->AcceptOnExpression(this);
        auto const *rhs{astBooleanBinaryExpression->GetRhs()};
        rhs->AcceptOnExpression(this);

        auto const type{UsualArithmeticConversions(
                lhs->GetDeducedType(), rhs->GetDeducedType()
        )};
        astBooleanBinaryExpression->SetUsualArithmeticConversionType(type);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstRelationalExpression const *astRelationalExpr
    ) {
        CmpTypeDeduction(astRelationalExpr);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstEqualityExpression const *astEqualityExpression
    ) {
        CmpTypeDeduction(astEqualityExpression);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstBitwiseAndExpression const *astBitwiseAndExpr
    ) {
        BinaryExprTypeDeduction(astBitwiseAndExpr);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstBitwiseXorExpression const *astBitwiseXorExpr
    ) {
        BinaryExprTypeDeduction(astBitwiseXorExpr);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstBitwiseOrExpression const *astBitwiseOrExpr
    ) {
        BinaryExprTypeDeduction(astBitwiseOrExpr);
    }

    void ExpressionTypeDeductionVisitor::Visit(
            parsing::AstLogicalAndExpression const *astLogicalAndExpr
    ) {
        CmpTypeDeduction(astLogicalAndExpr);
    }
}// namespace jcc::visitors
