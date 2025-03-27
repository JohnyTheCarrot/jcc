#include "macro_invoc_expected_lparen.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    MacroInvocExpectedLparen::MacroInvocExpectedLparen(
            std::shared_ptr<Source> source, mjolnir::Span macroNameSpan,
            mjolnir::Span nextTokenSpan
    )
        : DiagnosticData{std::move(source), nextTokenSpan.start(), mjolnir::BasicReportKind::Error}
        , m_MacroNameSpan{macroNameSpan}
        , m_NextTokenSpan{nextTokenSpan} {
    }

    void
    MacroInvocExpectedLparen::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
