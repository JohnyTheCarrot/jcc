#include "binary_operands_wrong_types.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    BinaryOperandsWrongTypes::BinaryOperandsWrongTypes(
            std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
            mjolnir::Span rhsSpan, mjolnir::Span opSpan, std::string message,
            parsing::types::ValueType const &lhsType,
            parsing::types::ValueType const &rhsType
    )
        : DiagnosticData{std::move(source), lhsSpan.start(), mjolnir::BasicReportKind::Error}
        , m_Message{std::move(message)}
        , m_LhsSpan{lhsSpan}
        , m_RhsSpan{rhsSpan}
        , m_OpSpan{opSpan}
        , m_LhsType{lhsType}
        , m_RhsType{rhsType} {
    }

    void BinaryOperandsWrongTypes::Visit(DiagnosticsVisitor const &visitor
    ) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
