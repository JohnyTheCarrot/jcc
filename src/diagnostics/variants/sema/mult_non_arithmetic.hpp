#ifndef SEMA_MULT_ARITHMETIC_H
#define SEMA_MULT_ARITHMETIC_H

#include "diagnostics/diagnostics.h"
#include "parsing_sema/types/type.h"

namespace jcc::diagnostics {
    class MultNonArithmetic final : public BinaryDiagnostic {
    public:
        MultNonArithmetic(
                std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
                mjolnir::Span rhsSpan, mjolnir::Span opSpan,
                parsing_sema::types::ValueType const &lhsType,
                parsing_sema::types::ValueType const &rhsType
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//SEMA_MULT_ARITHMETIC_H
