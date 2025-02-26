#include "unexpected_eof.hpp"

#include "visitors/diagnostics_visitor.hpp"

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
