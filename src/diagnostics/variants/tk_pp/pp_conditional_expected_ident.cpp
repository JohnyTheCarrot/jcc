#include "pp_conditional_expected_ident.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...
#include "mjolnir/span.hpp"                                     // for Span

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    PpConditionalExpectedIdent::PpConditionalExpectedIdent(
            std::shared_ptr<Source> source, mjolnir::Span condSpan,
            mjolnir::Span unexpectedTokenSpan
    )
        : DiagnosticData{std::move(source), unexpectedTokenSpan.start(), mjolnir::BasicReportKind::Error}
        , m_ConditionalSpan{condSpan}
        , m_UnexpectedTokenSpan{unexpectedTokenSpan} {
    }

    void PpConditionalExpectedIdent::Visit(DiagnosticsVisitor const &visitor
    ) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
