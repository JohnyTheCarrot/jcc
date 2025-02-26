#ifndef MACRO_ELLIPSIS_NOT_LAST_HPP
#define MACRO_ELLIPSIS_NOT_LAST_HPP
#include "diagnostics/diagnostics.h"

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
