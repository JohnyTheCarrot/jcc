#ifndef UNKNOWN_ESCAPE_SEQ_HPP
#define UNKNOWN_ESCAPE_SEQ_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct UnknownEscapeSeq final : DiagnosticData {
        mjolnir::Span m_Span;

        UnknownEscapeSeq(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UNKNOWN_ESCAPE_SEQ_HPP
