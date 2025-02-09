#ifndef CUSTOM_DIAGNOSTIC_HPP
#define CUSTOM_DIAGNOSTIC_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class CustomDiagnostic final : public DiagnosticData {
        mjolnir::Span m_Span;
        std::string   m_Message;

    public:
        CustomDiagnostic(
                std::shared_ptr<Source> source, mjolnir::Span span,
                std::string message, mjolnir::BasicReportKind reportKind
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//CUSTOM_DIAGNOSTIC_HPP
