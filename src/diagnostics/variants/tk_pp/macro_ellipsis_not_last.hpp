#ifndef MACRO_ELLIPSIS_NOT_LAST_HPP
#define MACRO_ELLIPSIS_NOT_LAST_HPP
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
    struct MacroEllipsisNotLast final : DiagnosticData {
        mjolnir::Span m_MacroSpan;
        mjolnir::Span m_CommaSpan;

        MacroEllipsisNotLast(
                std::shared_ptr<Source> source, mjolnir::Span macroSpan,
                mjolnir::Span commaSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//MACRO_ELLIPSIS_NOT_LAST_HPP
