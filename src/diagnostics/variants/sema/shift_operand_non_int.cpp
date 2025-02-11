#include "shift_operand_non_int.hpp"

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

    void ShiftOperandNonInt::Print(std::ostream &ostream) const {
        constexpr auto lhsColor{mjolnir::colors::light_cyan};
        constexpr auto rhsColor{mjolnir::colors::light_magenta};

        StartReport()
                .with_message(
                        "Both operands in a shift expression must be of "
                        "integer type."
                )
                .with_label(mjolnir::Label{GetOpSpan()})
                .with_label(
                        mjolnir::Label{GetLhsSpan()}
                                .with_message(
                                        std::format(
                                                "This is of type {}",
                                                lhsColor.fg(
                                                        GetLhsType().ToString()
                                                )
                                        )
                                )
                                .with_color(lhsColor)
                )
                .with_label(
                        mjolnir::Label{GetRhsSpan()}
                                .with_message(
                                        std::format(
                                                "This is of type {}",
                                                rhsColor.fg(
                                                        GetRhsType().ToString()
                                                )
                                        )
                                )
                                .with_color(rhsColor)
                )
                .with_help(
                        "Consider casting the operand(s) to integer type(s)."
                )
                .print(ostream);
    }
}// namespace jcc::diagnostics
