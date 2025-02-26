#ifndef INCLUDE_EXPECTED_HEADER_NAME_HPP
#define INCLUDE_EXPECTED_HEADER_NAME_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct IncludeExpectedHeaderName final : DiagnosticData {
        mjolnir::Span m_DirectiveSpan;
        mjolnir::Span m_HeaderNameSpan;

        IncludeExpectedHeaderName(
                std::shared_ptr<Source> source, mjolnir::Span directiveSpan,
                mjolnir::Span headerNameSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//INCLUDE_EXPECTED_HEADER_NAME_HPP
