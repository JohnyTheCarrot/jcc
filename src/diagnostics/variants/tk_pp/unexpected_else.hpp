#ifndef UNEXPECTED_ELSE_HPP
#define UNEXPECTED_ELSE_HPP

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
    struct UnexpectedElse final : DiagnosticData {
        mjolnir::Span m_ElseSpan;
        mjolnir::Span m_UnexpectedElseSpan;

        UnexpectedElse(
                std::shared_ptr<Source> source, mjolnir::Span elseSpan,
                mjolnir::Span unexpectedElseSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UNEXPECTED_ELSE_HPP
