#ifndef HEX_ESCAPE_SEQ_TOO_LARGE_HPP
#define HEX_ESCAPE_SEQ_TOO_LARGE_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct EscapeSeqTooLarge final : DiagnosticData {
        mjolnir::Span m_Span;

        EscapeSeqTooLarge(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//HEX_ESCAPE_SEQ_TOO_LARGE_HPP
