#ifndef TODO_HPP
#define TODO_HPP

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
    struct TodoError final : DiagnosticData {
        mjolnir::Span m_Span;

        TodoError(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//TODO_HPP
