#ifndef PP_CONDITIONAL_NOT_TERMINATED_HPP
#define PP_CONDITIONAL_NOT_TERMINATED_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct PpConditionalNotTerminated final : DiagnosticData {
        mjolnir::Span m_ConditionalSpan;
        mjolnir::Span m_EofSpan;

        PpConditionalNotTerminated(
                std::shared_ptr<Source> source, mjolnir::Span conditionalSpan,
                mjolnir::Span eofSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//PP_CONDITIONAL_NOT_TERMINATED_HPP
