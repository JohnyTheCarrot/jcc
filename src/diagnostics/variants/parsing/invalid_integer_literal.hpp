#ifndef INVALID_INTEGER_LITERAL_HPP
#define INVALID_INTEGER_LITERAL_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
  struct InvalidIntegerLiteral final : DiagnosticData {
    mjolnir::Span m_Span;

    InvalidIntegerLiteral(std::shared_ptr<Source> source, mjolnir::Span span);

    void Visit(DiagnosticsVisitor const &visitor) const override;
  };
}

#endif //INVALID_INTEGER_LITERAL_HPP
