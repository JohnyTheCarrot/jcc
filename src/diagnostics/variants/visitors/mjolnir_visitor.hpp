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
        void Print(ExpectedExpression const &diag) const override;
        void Print(UnexpectedEof const &diag) const override;
        void Print(UnexpectedChar const &diag) const override;
        void Print(IncludeOpenFailed const &diag) const override;
        void Print(IncludeExpectedHeaderName const &diag) const override;
        void Print(DirectiveExpectedNewline const &diag) const override;
        void Print(UndefExpectedIdent const &diag) const override;
        void Print(InvalidMacroParam const &diag) const override;
        void Print(MacroNameNotIdent const &diag) const override;
        void Print(UnexpectedElse const &diag) const override;
        void Print(ElseWithoutIf const &diag) const override;
        void Print(MacroExpectedCommaOrRparen const &diag) const override;
        void Print(MacroEllipsisNotLast const &diag) const override;
        void Print(MacroInvocExpectedLparen const &diag) const override;
        void Print(MacroInvocInvalidNumArgs const &diag) const override;
        void Print(UnrecognizedIntegerSuffix const &diag) const override;
        void Print(UnrecognizedFloatingSuffix const &diag) const override;
        void Print(InvalidFloatingPointLiteral const &diag) const override;
        void Print(InvalidIntegerLiteral const &diag) const override;
        void Print(BasicSyntaxError const &diag) const override;
        void Print(NoCompatIntType const &diag) const override;
        void Print(InvalidSpecifierQualifierList const &diag) const override;
    };
}// namespace jcc::diagnostics
#endif//MJOLNIR_VISITOR_HPP
