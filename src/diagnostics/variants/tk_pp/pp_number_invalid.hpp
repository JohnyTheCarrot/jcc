#ifndef PP_NUMBER_INVALID_HPP
#define PP_NUMBER_INVALID_HPP

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
    struct PpNumberInvalid final : DiagnosticData {
        mjolnir::Span m_Span;

        PpNumberInvalid(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//PP_NUMBER_INVALID_HPP
