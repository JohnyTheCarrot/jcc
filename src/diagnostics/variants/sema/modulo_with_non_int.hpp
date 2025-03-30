#ifndef SEMA_MODULO_INT_H
#define SEMA_MODULO_INT_H

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct ModuloWithNonInt final : BinaryDiagnostic {
        ModuloWithNonInt(
                std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
                mjolnir::Span rhsSpan, mjolnir::Span opSpan,
                parsing::types::ValueType const &lhsType,
                parsing::types::ValueType const &rhsType
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//SEMA_MODULO_INT_H
