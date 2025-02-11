#ifndef MULTI_BYTE_CHAR_IMPL_DEF_HPP
#define MULTI_BYTE_CHAR_IMPL_DEF_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class MultiByteCharImplDef final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        MultiByteCharImplDef(
                std::shared_ptr<Source> source, mjolnir::Span span
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//MULTI_BYTE_CHAR_IMPL_DEF_HPP
