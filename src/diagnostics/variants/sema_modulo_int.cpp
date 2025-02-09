#include "sema_modulo_int.h"

#include <utility>

namespace jcc::diagnostics {
    SemaModuloInt::SemaModuloInt(
            std::shared_ptr<Source> source, mjolnir::Span const &lhsSpan,
            mjolnir::Span const &rhsSpan, mjolnir::Span const &opSpan,
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

    void SemaModuloInt::Print(std::ostream &ostream) const {
        constexpr auto lhsColor{mjolnir::colors::light_cyan};
        constexpr auto rhsColor{mjolnir::colors::light_magenta};

        StartReport().with_message(
                      "The operands of a modulo expression must both be "
                      "integer types."
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
