#include "else_without_if.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    ElseWithoutIf::ElseWithoutIf(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{std::move(span)} {
    }

    void ElseWithoutIf::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
