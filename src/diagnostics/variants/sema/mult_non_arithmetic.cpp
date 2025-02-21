#include "mult_non_arithmetic.hpp"

#include <utility>

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    MultNonArithmetic::MultNonArithmetic(
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

    void MultNonArithmetic::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
