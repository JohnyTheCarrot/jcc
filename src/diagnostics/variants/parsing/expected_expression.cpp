#include "expected_expression.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    ExpectedExpression::ExpectedExpression(
            std::shared_ptr<Source> source, mjolnir::Span lhsSpan,
            mjolnir::Span opSpan, std::optional<mjolnir::Span> const &rhsSpan
    )
        : DiagnosticData{std::move(source), rhsSpan.value_or(opSpan).start(), mjolnir::BasicReportKind::Error}
        , m_LhsSpan{lhsSpan}
        , m_OpSpan{opSpan}
        , m_RhsSpan{rhsSpan} {
    }

    void ExpectedExpression::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
