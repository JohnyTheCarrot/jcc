#include "invalid_macro_param.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    InvalidMacroParam::InvalidMacroParam(
            std::shared_ptr<Source> source, mjolnir::Span macroDefSpan,
            mjolnir::Span invalidParamSpan
    )
        : DiagnosticData{std::move(source), invalidParamSpan.start(), mjolnir::BasicReportKind::Error}
        , m_MacroDefSpan{macroDefSpan}
        , m_InvalidParamSpan{invalidParamSpan} {
    }

    void InvalidMacroParam::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
