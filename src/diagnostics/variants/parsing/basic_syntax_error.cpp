#include "basic_syntax_error.hpp"

#include <mjolnir/report.hpp>

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    BasicSyntaxError::BasicSyntaxError(
            std::shared_ptr<Source> source, mjolnir::Span span,
            std::string message
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span}
        , m_Message{std::move(message)} {
    }

    BasicSyntaxError::BasicSyntaxError(
            std::shared_ptr<Source> source, mjolnir::Span span,
            std::string_view expected, std::string_view received
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span}
        , m_Message{ExpectedReceivedPair{expected, received}} {
    }

    void BasicSyntaxError::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
