#include "unrecognized_integer_suffix.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    UnrecognizedIntegerSuffix::UnrecognizedIntegerSuffix(
            std::shared_ptr<Source> source, mjolnir::Span suffixSpan
    )
        : DiagnosticData{std::move(source), suffixSpan.start(), mjolnir::BasicReportKind::Error}
        , m_SuffixSpan{suffixSpan} {
    }

    void
    UnrecognizedIntegerSuffix::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
