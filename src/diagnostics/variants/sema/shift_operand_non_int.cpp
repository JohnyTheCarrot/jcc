#include "shift_operand_non_int.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    ShiftOperandNonInt::ShiftOperandNonInt(
            std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
            mjolnir::Span rhsSpan, mjolnir::Span opSpan,
            parsing_sema::types::ValueType const &lhsType,
            parsing_sema::types::ValueType const &rhsType
    )
        : BinaryDiagnostic{
                  mjolnir::BasicReportKind::Error,
                  std::move(source),
                  lhsSpan,
                  rhsSpan,
                  opSpan,
                  lhsType,
                  rhsType
          } {
    }

    void ShiftOperandNonInt::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
