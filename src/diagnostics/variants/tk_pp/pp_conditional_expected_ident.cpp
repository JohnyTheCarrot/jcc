#include "pp_conditional_expected_ident.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    PpConditionalExpectedIdent::PpConditionalExpectedIdent(
            std::shared_ptr<Source> source, mjolnir::Span condSpan,
            mjolnir::Span unexpectedTokenSpan
    )
        : DiagnosticData{std::move(source), unexpectedTokenSpan.start(), mjolnir::BasicReportKind::Error}
        , m_ConditionalSpan{condSpan}
        , m_UnexpectedTokenSpan{unexpectedTokenSpan} {
    }

    void
    PpConditionalExpectedIdent::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
