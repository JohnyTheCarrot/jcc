#include "macro_invoc_expected_lparen.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    MacroInvocExpectedLparen::MacroInvocExpectedLparen(
            std::shared_ptr<Source> source, mjolnir::Span macroNameSpan,
            mjolnir::Span nextTokenSpan
    )
        : DiagnosticData{std::move(source), nextTokenSpan.start(), mjolnir::BasicReportKind::Error}
        , m_MacroNameSpan{macroNameSpan}
        , m_NextTokenSpan{nextTokenSpan} {
    }

    void MacroInvocExpectedLparen::Visit(DiagnosticsVisitor const &visitor
    ) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
