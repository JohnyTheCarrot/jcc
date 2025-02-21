#ifndef UNTERMED_STRING_H
#define UNTERMED_STRING_H

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct UntermedString final : DiagnosticData {
        mjolnir::Span m_Span;

        UntermedString(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UNTERMED_STRING_H
