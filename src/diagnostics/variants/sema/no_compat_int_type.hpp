#ifndef NO_COMPAT_INT_TYPE_HPP
#define NO_COMPAT_INT_TYPE_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct NoCompatIntType final : DiagnosticData {
        mjolnir::Span m_Span;

        NoCompatIntType(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//NO_COMPAT_INT_TYPE_HPP
