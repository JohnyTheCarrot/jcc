#include "char_const_empty.h"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    CharConstEmpty::CharConstEmpty(
            std::shared_ptr<Source> source, mjolnir::Span span,
            std::optional<mjolnir::Span> const &potentiallyClosingQuote
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span}
        , m_PotentiallyClosingQuote{potentiallyClosingQuote} {
    }

    void CharConstEmpty::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
