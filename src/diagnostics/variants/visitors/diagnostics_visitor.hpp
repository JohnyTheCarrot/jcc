#ifndef DIAGNOSTICS_VISITOR_HPP
#define DIAGNOSTICS_VISITOR_HPP

#include <ostream>// for ostream

namespace jcc::diagnostics {
#pragma region Forward declarations
    struct NoCompatIntType;
    struct InvalidSpecifierQualifierList;
    struct BasicSyntaxError;
    struct InvalidIntegerLiteral;
    struct InvalidFloatingPointLiteral;
    struct UndefExpectedIdent;
    struct UnrecognizedFloatingSuffix;
    struct UnrecognizedIntegerSuffix;
    struct MacroInvocInvalidNumArgs;
    struct MacroInvocExpectedLparen;
    struct MacroEllipsisNotLast;
    struct MacroExpectedCommaOrRparen;
    struct ElseWithoutIf;
    struct UnexpectedElse;
    struct MacroNameNotIdent;
    struct InvalidMacroParam;
    struct DirectiveExpectedNewline;
    struct IncludeExpectedHeaderName;
    struct IncludeOpenFailed;
    struct UnexpectedChar;
    struct UnexpectedEof;
    struct ExpectedExpression;
    struct BinaryOperandsWrongTypes;
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

#pragma endregion

    class DiagnosticsVisitor {
        std::ostream *m_Ostream;

    protected:
        [[nodiscard]]
        std::ostream &GetOstream() const;

    public:
        explicit DiagnosticsVisitor(std::ostream &ostream);

        virtual ~DiagnosticsVisitor() = default;

        virtual void Print(UnknownEscapeSeq const &diag) const              = 0;
        virtual void Print(Utf8TooManyChars const &diag) const              = 0;
        virtual void Print(UntermedString const &diag) const                = 0;
        virtual void Print(UntermedChar const &diag) const                  = 0;
        virtual void Print(PpConditionalNotTerminated const &diag) const    = 0;
        virtual void Print(PpConditionalExpectedIdent const &diag) const    = 0;
        virtual void Print(OrphanedEndif const &diag) const                 = 0;
        virtual void Print(PpNumberInvalid const &diag) const               = 0;
        virtual void Print(TodoError const &diag) const                     = 0;
        virtual void Print(MultiByteCharImplDef const &diag) const          = 0;
        virtual void Print(IllegalMacroRedef const &diag) const             = 0;
        virtual void Print(HexEscapeEmpty const &diag) const                = 0;
        virtual void Print(EscapeWithoutNewline const &diag) const          = 0;
        virtual void Print(EscapeSeqTooLarge const &diag) const             = 0;
        virtual void Print(DiagnosticDirectiveNoTokens const &diag) const   = 0;
        virtual void Print(CustomDiagnostic const &diag) const              = 0;
        virtual void Print(CharConstGt4Chars const &diag) const             = 0;
        virtual void Print(CharConstEmpty const &diag) const                = 0;
        virtual void Print(BinaryOperandsWrongTypes const &diag) const      = 0;
        virtual void Print(ExpectedExpression const &diag) const            = 0;
        virtual void Print(UnexpectedEof const &diag) const                 = 0;
        virtual void Print(UnexpectedChar const &diag) const                = 0;
        virtual void Print(IncludeOpenFailed const &diag) const             = 0;
        virtual void Print(IncludeExpectedHeaderName const &diag) const     = 0;
        virtual void Print(DirectiveExpectedNewline const &diag) const      = 0;
        virtual void Print(UndefExpectedIdent const &diag) const            = 0;
        virtual void Print(InvalidMacroParam const &diag) const             = 0;
        virtual void Print(MacroNameNotIdent const &diag) const             = 0;
        virtual void Print(UnexpectedElse const &diag) const                = 0;
        virtual void Print(ElseWithoutIf const &diag) const                 = 0;
        virtual void Print(MacroExpectedCommaOrRparen const &diag) const    = 0;
        virtual void Print(MacroEllipsisNotLast const &diag) const          = 0;
        virtual void Print(MacroInvocExpectedLparen const &diag) const      = 0;
        virtual void Print(MacroInvocInvalidNumArgs const &diag) const      = 0;
        virtual void Print(UnrecognizedIntegerSuffix const &diag) const     = 0;
        virtual void Print(UnrecognizedFloatingSuffix const &diag) const    = 0;
        virtual void Print(InvalidFloatingPointLiteral const &diag) const   = 0;
        virtual void Print(InvalidIntegerLiteral const &diag) const         = 0;
        virtual void Print(BasicSyntaxError const &diag) const              = 0;
        virtual void Print(NoCompatIntType const &diag) const               = 0;
        virtual void Print(InvalidSpecifierQualifierList const &diag) const = 0;
    };
}// namespace jcc::diagnostics

#endif//DIAGNOSTICS_VISITOR_HPP
