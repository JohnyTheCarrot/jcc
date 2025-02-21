#ifndef PP_NUMBER_INVALID_HPP
#define PP_NUMBER_INVALID_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct PpNumberInvalid final : DiagnosticData {
        mjolnir::Span m_Span;

        PpNumberInvalid(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//PP_NUMBER_INVALID_HPP
