#ifndef DIAGNOSTICS_VISITOR_HPP
#define DIAGNOSTICS_VISITOR_HPP

#include <ostream>

namespace jcc::diagnostics {
    struct ModuloWithNonInt;
}

namespace jcc::diagnostics {
    struct MultNonArithmetic;
}

namespace jcc::diagnostics {
    struct ShiftOperandNonInt;
    struct CharConstEmpty;
    struct CharConstGt4Chars;
    struct CustomDiagnostic;
    struct DiagnosticDirectiveNoTokens;
    struct EscapeSeqTooLarge;
    struct EscapeWithoutNewline;
    struct HexEscapeEmpty;
    struct IllegalMacroRedef;
    struct MultiByteCharImplDef;
    struct TodoError;
    struct PpNumberInvalid;
    struct OrphanedEndif;
    struct PpConditionalExpectedIdent;
    struct PpConditionalNotTerminated;
    struct UnknownEscapeSeq;
    struct UntermedChar;
    struct UntermedString;
    struct Utf8TooManyChars;

    class DiagnosticsVisitor {
        std::ostream *m_Ostream;

    protected:
        [[nodiscard]]
        std::ostream &GetOstream() const;

    public:
        explicit DiagnosticsVisitor(std::ostream &ostream);

        virtual ~DiagnosticsVisitor() = default;

        virtual void Print(UnknownEscapeSeq const &diag) const            = 0;
        virtual void Print(Utf8TooManyChars const &diag) const            = 0;
        virtual void Print(UntermedString const &diag) const              = 0;
        virtual void Print(UntermedChar const &diag) const                = 0;
        virtual void Print(PpConditionalNotTerminated const &diag) const  = 0;
        virtual void Print(PpConditionalExpectedIdent const &diag) const  = 0;
        virtual void Print(OrphanedEndif const &diag) const               = 0;
        virtual void Print(PpNumberInvalid const &diag) const             = 0;
        virtual void Print(TodoError const &diag) const                   = 0;
        virtual void Print(MultiByteCharImplDef const &diag) const        = 0;
        virtual void Print(IllegalMacroRedef const &diag) const           = 0;
        virtual void Print(HexEscapeEmpty const &diag) const              = 0;
        virtual void Print(EscapeWithoutNewline const &diag) const        = 0;
        virtual void Print(EscapeSeqTooLarge const &diag) const           = 0;
        virtual void Print(DiagnosticDirectiveNoTokens const &diag) const = 0;
        virtual void Print(CustomDiagnostic const &diag) const            = 0;
        virtual void Print(CharConstGt4Chars const &diag) const           = 0;
        virtual void Print(CharConstEmpty const &diag) const              = 0;
        virtual void Print(ShiftOperandNonInt const &diag) const          = 0;
        virtual void Print(MultNonArithmetic const &diag) const           = 0;
        virtual void Print(ModuloWithNonInt const &diag) const            = 0;
    };
}// namespace jcc::diagnostics

#endif//DIAGNOSTICS_VISITOR_HPP
