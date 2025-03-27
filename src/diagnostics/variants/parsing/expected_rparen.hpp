#ifndef EXPECTED_RPAREN_HPP
#define EXPECTED_RPAREN_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct ExpectedRParen final : DiagnosticData {
        mjolnir::Span m_Span;

        ExpectedRParen(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//EXPECTED_RPAREN_HPP
