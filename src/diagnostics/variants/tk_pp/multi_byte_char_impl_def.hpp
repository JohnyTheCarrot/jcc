#ifndef MULTI_BYTE_CHAR_IMPL_DEF_HPP
#define MULTI_BYTE_CHAR_IMPL_DEF_HPP

#include <memory>          // for shared_ptr
#include <mjolnir/span.hpp>// for Span

#include "diagnostics/diagnostics.h"// for DiagnosticData

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

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
