#ifndef ELSE_WITHOUT_IF_HPP
#define ELSE_WITHOUT_IF_HPP
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
    struct ElseWithoutIf final : DiagnosticData {
        mjolnir::Span m_Span;

        ElseWithoutIf(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//ELSE_WITHOUT_IF_HPP
