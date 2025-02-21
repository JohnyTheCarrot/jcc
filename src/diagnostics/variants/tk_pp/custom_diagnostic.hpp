#ifndef CUSTOM_DIAGNOSTIC_HPP
#define CUSTOM_DIAGNOSTIC_HPP

#include "diagnostics/diagnostics.h"

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
