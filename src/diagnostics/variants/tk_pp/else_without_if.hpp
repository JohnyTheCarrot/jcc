#ifndef ELSE_WITHOUT_IF_HPP
#define ELSE_WITHOUT_IF_HPP
#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct ElseWithoutIf final : DiagnosticData {
        mjolnir::Span m_Span;

        ElseWithoutIf(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//ELSE_WITHOUT_IF_HPP
