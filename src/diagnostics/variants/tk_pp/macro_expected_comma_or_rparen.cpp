#include "macro_expected_comma_or_rparen.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    MacroExpectedCommaOrRparen::MacroExpectedCommaOrRparen(
            std::shared_ptr<Source> source, mjolnir::Span macroSpan,
            mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_MacroSpan{macroSpan}
        , m_Span{span} {
    }

    void MacroExpectedCommaOrRparen::Visit(DiagnosticsVisitor const &visitor
    ) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
