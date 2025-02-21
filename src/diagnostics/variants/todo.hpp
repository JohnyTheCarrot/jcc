#ifndef TODO_HPP
#define TODO_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct TodoError final : DiagnosticData {
        mjolnir::Span m_Span;

        TodoError(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//TODO_HPP
