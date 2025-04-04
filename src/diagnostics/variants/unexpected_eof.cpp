#include "unexpected_eof.hpp"

#include <utility>// for move

#include "mjolnir/report.hpp"              // for BasicReportKind
#include "visitors/diagnostics_visitor.hpp"// for DiagnosticsVisitor

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    UnexpectedEof::UnexpectedEof(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void UnexpectedEof::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
