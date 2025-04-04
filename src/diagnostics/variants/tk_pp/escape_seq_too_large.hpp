#ifndef HEX_ESCAPE_SEQ_TOO_LARGE_HPP
#define HEX_ESCAPE_SEQ_TOO_LARGE_HPP

#include <memory>// for shared_ptr

#include "diagnostics/diagnostics.h"// for DiagnosticData
#include "mjolnir/span.hpp"         // for Span

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    struct EscapeSeqTooLarge final : DiagnosticData {
        mjolnir::Span m_Span;

        EscapeSeqTooLarge(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//HEX_ESCAPE_SEQ_TOO_LARGE_HPP
