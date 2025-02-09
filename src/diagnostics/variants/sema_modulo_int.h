#ifndef SEMA_MODULO_INT_H
#define SEMA_MODULO_INT_H

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class SemaModuloInt final : public BinaryDiagnostic {
    public:
        SemaModuloInt(
                std::shared_ptr<Source> source, mjolnir::Span const &lhsSpan,
                mjolnir::Span const &rhsSpan, mjolnir::Span const &opSpan,
                parsing_sema::types::ValueType const &lhsType,
                parsing_sema::types::ValueType const &rhsType
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//SEMA_MODULO_INT_H
