#include "utf8_too_many_chars.hpp"

#include <utility>

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    Utf8TooManyChars::Utf8TooManyChars(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void Utf8TooManyChars::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
