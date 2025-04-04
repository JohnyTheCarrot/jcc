#include "custom_diagnostic.hpp"

#include <utility>// for move

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"// for Dia...

namespace jcc {
    namespace diagnostics {
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace mjolnir {
    enum class BasicReportKind;
}// namespace mjolnir

namespace jcc::diagnostics {
    CustomDiagnostic::CustomDiagnostic(
            std::shared_ptr<Source> source, mjolnir::Span span,
            std::string message, mjolnir::BasicReportKind reportKind
    )
        : DiagnosticData{std::move(source), span.start(), reportKind}
        , m_Span{span}
        , m_Message{std::move(message)} {
    }

    void CustomDiagnostic::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
