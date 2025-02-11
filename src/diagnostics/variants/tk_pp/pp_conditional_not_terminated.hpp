#ifndef PP_CONDITIONAL_NOT_TERMINATED_HPP
#define PP_CONDITIONAL_NOT_TERMINATED_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class PpConditionalNotTerminated final : public DiagnosticData {
        mjolnir::Span m_ConditionalSpan;
        mjolnir::Span m_EofSpan;

    public:
        PpConditionalNotTerminated(
                std::shared_ptr<Source> source, mjolnir::Span conditionalSpan,
                mjolnir::Span eofSpan
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//PP_CONDITIONAL_NOT_TERMINATED_HPP
