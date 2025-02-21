#ifndef UNTERMED_CHAR_H
#define UNTERMED_CHAR_H

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct UntermedChar final : DiagnosticData {
        mjolnir::Span m_Span;

        UntermedChar(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UNTERMED_CHAR_H
