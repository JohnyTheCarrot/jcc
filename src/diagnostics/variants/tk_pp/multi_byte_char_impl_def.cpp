#include "multi_byte_char_impl_def.hpp"

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
    MultiByteCharImplDef::MultiByteCharImplDef(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Warning}
        , m_Span{span} {
    }

    void MultiByteCharImplDef::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
