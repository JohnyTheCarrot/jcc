#ifndef SEMA_MULT_ARITHMETIC_H
#define SEMA_MULT_ARITHMETIC_H

#include "diagnostics/diagnostics.h"
#include "parsing/types/type.h"

namespace jcc::diagnostics {
    struct BinaryOperandsWrongTypes final : DiagnosticData {
        std::string               m_Message;
        mjolnir::Span             m_LhsSpan;
        mjolnir::Span             m_RhsSpan;
        mjolnir::Span             m_OpSpan;
        parsing::types::ValueType m_LhsType;
        parsing::types::ValueType m_RhsType;

        BinaryOperandsWrongTypes(
                std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
                mjolnir::Span rhsSpan, mjolnir::Span opSpan,
                std::string message, parsing::types::ValueType const &lhsType,
                parsing::types::ValueType const &rhsType
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//SEMA_MULT_ARITHMETIC_H
