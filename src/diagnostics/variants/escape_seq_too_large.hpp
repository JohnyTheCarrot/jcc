#ifndef HEX_ESCAPE_SEQ_TOO_LARGE_HPP
#define HEX_ESCAPE_SEQ_TOO_LARGE_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class EscapeSeqTooLarge final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        EscapeSeqTooLarge(std::shared_ptr<Source> source, mjolnir::Span span);

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//HEX_ESCAPE_SEQ_TOO_LARGE_HPP
