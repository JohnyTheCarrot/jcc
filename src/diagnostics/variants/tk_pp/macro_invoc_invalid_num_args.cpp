#include "macro_invoc_invalid_num_args.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    MacroInvocInvalidNumArgs::MacroInvocInvalidNumArgs(
            std::shared_ptr<Source> source, mjolnir::Span macroInvocSpan,
            mjolnir::Span macroArgsSpan, int expectedNArgs, int receivedNArgs,
            bool isMinimum
    )
        : DiagnosticData{std::move(source), macroArgsSpan.start(), mjolnir::BasicReportKind::Error}
        , m_MacroInvocSpan{macroInvocSpan}
        , m_MacroArgsSpan{macroArgsSpan}
        , m_ExpectedNArgs{expectedNArgs}
        , m_ReceivedNArgs{receivedNArgs}
        , m_IsMinimum{isMinimum} {
    }

    void
    MacroInvocInvalidNumArgs::Visit(DiagnosticsVisitor const &visitor) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
