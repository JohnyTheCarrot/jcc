#ifndef DIRECTIVE_EXPECTED_NEWLINE_HPP
#define DIRECTIVE_EXPECTED_NEWLINE_HPP
#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct DirectiveExpectedNewline final : DiagnosticData {
        mjolnir::Span m_DirectiveSpan;
        mjolnir::Span m_NextTokenSpan;

        DirectiveExpectedNewline(
                std::shared_ptr<Source> source, mjolnir::Span directiveSpan,
                mjolnir::Span nextTokenSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//DIRECTIVE_EXPECTED_NEWLINE_HPP
