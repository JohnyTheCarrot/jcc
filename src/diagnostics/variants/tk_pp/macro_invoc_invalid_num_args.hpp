#ifndef MACRO_INVOC_INVALID_NUM_ARGS_HPP
#define MACRO_INVOC_INVALID_NUM_ARGS_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct MacroInvocInvalidNumArgs final : DiagnosticData {
        mjolnir::Span m_MacroInvocSpan;
        mjolnir::Span m_MacroArgsSpan;
        int           m_ExpectedNArgs;
        int           m_ReceivedNArgs;
        bool          m_IsMinimum;

        MacroInvocInvalidNumArgs(
                std::shared_ptr<Source> source, mjolnir::Span macroInvocSpan,
                mjolnir::Span macroArgsSpan, int expectedNArgs,
                int receivedNArgs, bool isMinimum
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//MACRO_INVOC_INVALID_NUM_ARGS_HPP
