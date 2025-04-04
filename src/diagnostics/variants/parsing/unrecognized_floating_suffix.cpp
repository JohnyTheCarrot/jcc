#include "unrecognized_floating_suffix.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...
#include "mjolnir/report.hpp"                                   // for Bas...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    UnrecognizedFloatingSuffix::UnrecognizedFloatingSuffix(
            std::shared_ptr<Source> source, mjolnir::Span suffixSpan
    )
        : DiagnosticData{std::move(source), suffixSpan.start(), mjolnir::BasicReportKind::Error}
        , m_SuffixSpan{suffixSpan} {
    }

    void UnrecognizedFloatingSuffix::Visit(DiagnosticsVisitor const &visitor
    ) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
