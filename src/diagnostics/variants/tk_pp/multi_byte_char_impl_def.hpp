#ifndef MULTI_BYTE_CHAR_IMPL_DEF_HPP
#define MULTI_BYTE_CHAR_IMPL_DEF_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct MultiByteCharImplDef final : DiagnosticData {
        mjolnir::Span m_Span;

        MultiByteCharImplDef(
                std::shared_ptr<Source> source, mjolnir::Span span
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//MULTI_BYTE_CHAR_IMPL_DEF_HPP
