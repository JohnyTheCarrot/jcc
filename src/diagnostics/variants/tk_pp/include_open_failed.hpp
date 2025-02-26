#ifndef INCLUDE_OPEN_FAILED_HPP
#define INCLUDE_OPEN_FAILED_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct IncludeOpenFailed final : DiagnosticData {
        mjolnir::Span m_DirectiveSpan;
        mjolnir::Span m_HeaderNameSpan;

        IncludeOpenFailed(
                std::shared_ptr<Source> source, mjolnir::Span directiveSpan,
                mjolnir::Span headerNameSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//INCLUDE_OPEN_FAILED_HPP
