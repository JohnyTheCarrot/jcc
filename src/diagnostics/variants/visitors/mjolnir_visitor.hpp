#ifndef MJOLNIR_VISITOR_HPP
#define MJOLNIR_VISITOR_HPP

#include "diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    class MjolnirVisitor final : public DiagnosticsVisitor {
    public:
        using DiagnosticsVisitor::DiagnosticsVisitor;

        void Print(UnknownEscapeSeq const &diag) const override;
        void Print(Utf8TooManyChars const &diag) const override;
        void Print(UntermedString const &diag) const override;
        void Print(UntermedChar const &diag) const override;
        void Print(PpConditionalNotTerminated const &diag) const override;
        void Print(PpConditionalExpectedIdent const &diag) const override;
        void Print(OrphanedEndif const &diag) const override;
        void Print(PpNumberInvalid const &diag) const override;
        void Print(TodoError const &diag) const override;
        void Print(MultiByteCharImplDef const &diag) const override;
        void Print(IllegalMacroRedef const &diag) const override;
        void Print(HexEscapeEmpty const &diag) const override;
        void Print(EscapeWithoutNewline const &diag) const override;
        void Print(EscapeSeqTooLarge const &diag) const override;
        void Print(DiagnosticDirectiveNoTokens const &diag) const override;
        void Print(CustomDiagnostic const &diag) const override;
        void Print(CharConstGt4Chars const &diag) const override;
        void Print(CharConstEmpty const &diag) const override;
        void Print(ShiftOperandNonInt const &diag) const override;
        void Print(MultNonArithmetic const &diag) const override;
        void Print(ModuloWithNonInt const &diag) const override;
    };
}// namespace jcc::diagnostics
#endif//MJOLNIR_VISITOR_HPP
