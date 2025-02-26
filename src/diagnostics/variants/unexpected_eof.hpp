#ifndef UNEXPECTED_EOF_HPP
#define UNEXPECTED_EOF_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct UnexpectedEof final : DiagnosticData {
        mjolnir::Span m_Span;

        UnexpectedEof(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UNEXPECTED_EOF_HPP
