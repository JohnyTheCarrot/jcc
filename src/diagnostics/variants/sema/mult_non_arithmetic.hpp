#ifndef SEMA_MULT_ARITHMETIC_H
#define SEMA_MULT_ARITHMETIC_H

#include "diagnostics/diagnostics.h"
#include "parsing_sema/types/type.h"

namespace jcc::diagnostics {
    struct MultNonArithmetic final : BinaryDiagnostic {
        MultNonArithmetic(
                std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
                mjolnir::Span rhsSpan, mjolnir::Span opSpan,
                parsing_sema::types::ValueType const &lhsType,
                parsing_sema::types::ValueType const &rhsType
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//SEMA_MULT_ARITHMETIC_H
