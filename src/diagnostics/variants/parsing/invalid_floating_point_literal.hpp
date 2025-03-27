#ifndef INVALID_FLOATING_POINT_LITERAL_HPP
#define INVALID_FLOATING_POINT_LITERAL_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
  struct InvalidFloatingPointLiteral final : DiagnosticData {
    mjolnir::Span m_Span;

    InvalidFloatingPointLiteral(std::shared_ptr<Source> source, mjolnir::Span span);

    void Visit(DiagnosticsVisitor const &visitor) const override;
  };
}

#endif //INVALID_FLOATING_POINT_LITERAL_HPP
