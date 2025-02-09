#ifndef SEMA_MULT_ARITHMETIC_H
#define SEMA_MULT_ARITHMETIC_H

#include "diagnostics/diagnostics.h"
#include "parsing_sema/types/type.h"

namespace jcc::diagnostics {
    class SemaMultArithmetic final : public BinaryDiagnostic {
    public:
        SemaMultArithmetic(
                std::shared_ptr<Source> source, mjolnir::Span const &lhsSpan,
                mjolnir::Span const &rhsSpan, mjolnir::Span const &opSpan,
                parsing_sema::types::ValueType const &lhsType,
                parsing_sema::types::ValueType const &rhsType
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//SEMA_MULT_ARITHMETIC_H
