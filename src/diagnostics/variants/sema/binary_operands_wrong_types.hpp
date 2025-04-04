#ifndef SEMA_MULT_ARITHMETIC_H
#define SEMA_MULT_ARITHMETIC_H

#include <memory>// for shared_ptr
#include <string>// for string

#include "diagnostics/diagnostics.h"// for DiagnosticData
#include "mjolnir/span.hpp"         // for Span
#include "parsing/types/type.h"     // for ValueType

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

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
