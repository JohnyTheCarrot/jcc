#ifndef SHIFT_OPERAND_NON_INT_HPP
#define SHIFT_OPERAND_NON_INT_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class ShiftOperandNonInt final : public BinaryDiagnostic {
    public:
        ShiftOperandNonInt(
                std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
                mjolnir::Span rhsSpan, mjolnir::Span opSpan,
                parsing_sema::types::ValueType const &lhsType,
                parsing_sema::types::ValueType const &rhsType
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//SHIFT_OPERAND_NON_INT_HPP
