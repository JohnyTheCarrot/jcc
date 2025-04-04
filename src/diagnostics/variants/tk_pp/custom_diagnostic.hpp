#ifndef CUSTOM_DIAGNOSTIC_HPP
#define CUSTOM_DIAGNOSTIC_HPP

#include <memory>// for shared_ptr
#include <string>// for string

#include "diagnostics/diagnostics.h"// for DiagnosticData
#include "mjolnir/span.hpp"         // for Span

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace mjolnir {
    enum class BasicReportKind;
}// namespace mjolnir

namespace jcc::diagnostics {
    struct CustomDiagnostic final : DiagnosticData {
        mjolnir::Span m_Span;
        std::string   m_Message;

        CustomDiagnostic(
                std::shared_ptr<Source> source, mjolnir::Span span,
                std::string message, mjolnir::BasicReportKind reportKind
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//CUSTOM_DIAGNOSTIC_HPP
