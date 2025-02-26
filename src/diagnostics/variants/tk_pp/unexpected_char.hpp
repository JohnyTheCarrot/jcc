#ifndef UNEXPECTED_CHAR_HPP
#define UNEXPECTED_CHAR_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct UnexpectedChar final : DiagnosticData {
        mjolnir::Span m_Span;

        UnexpectedChar(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UNEXPECTED_CHAR_HPP
