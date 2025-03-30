#include "mjolnir_visitor.hpp"

#include <diagnostics/variants/parsing/invalid_floating_point_literal.hpp>
#include <diagnostics/variants/parsing/invalid_integer_literal.hpp>
#include <diagnostics/variants/parsing/unrecognized_floating_suffix.hpp>
#include <diagnostics/variants/parsing/unrecognized_integer_suffix.hpp>
#include <diagnostics/variants/sema/no_compat_int_type.hpp>
#include <mjolnir/report.hpp>

#include "diagnostics/variants/parsing/basic_syntax_error.hpp"
#include "diagnostics/variants/parsing/expected_expression.hpp"
#include "diagnostics/variants/sema/invalid_specifier_qualifier_list.hpp"
#include "diagnostics/variants/sema/modulo_with_non_int.hpp"
#include "diagnostics/variants/sema/mult_non_arithmetic.hpp"
#include "diagnostics/variants/sema/shift_operand_non_int.hpp"
#include "diagnostics/variants/tk_pp/char_const_empty.h"
#include "diagnostics/variants/tk_pp/char_const_gt_4_chars.hpp"
#include "diagnostics/variants/tk_pp/custom_diagnostic.hpp"
#include "diagnostics/variants/tk_pp/diagnostic_directive_no_tokens.hpp"
#include "diagnostics/variants/tk_pp/directive_expected_newline.hpp"
#include "diagnostics/variants/tk_pp/else_without_if.hpp"
#include "diagnostics/variants/tk_pp/escape_seq_too_large.hpp"
#include "diagnostics/variants/tk_pp/escape_without_newline.hpp"
#include "diagnostics/variants/tk_pp/hex_escape_empty.hpp"
#include "diagnostics/variants/tk_pp/illegal_macro_redef.hpp"
#include "diagnostics/variants/tk_pp/include_expected_header_name.hpp"
#include "diagnostics/variants/tk_pp/include_open_failed.hpp"
#include "diagnostics/variants/tk_pp/invalid_macro_param.hpp"
#include "diagnostics/variants/tk_pp/macro_ellipsis_not_last.hpp"
#include "diagnostics/variants/tk_pp/macro_expected_comma_or_rparen.hpp"
#include "diagnostics/variants/tk_pp/macro_invoc_expected_lparen.hpp"
#include "diagnostics/variants/tk_pp/macro_invoc_invalid_num_args.hpp"
#include "diagnostics/variants/tk_pp/macro_name_not_ident.hpp"
#include "diagnostics/variants/tk_pp/multi_byte_char_impl_def.hpp"
#include "diagnostics/variants/tk_pp/orphaned_endif.hpp"
#include "diagnostics/variants/tk_pp/pp_conditional_expected_ident.hpp"
#include "diagnostics/variants/tk_pp/pp_conditional_not_terminated.hpp"
#include "diagnostics/variants/tk_pp/pp_number_invalid.hpp"
#include "diagnostics/variants/tk_pp/undef_expected_ident.hpp"
#include "diagnostics/variants/tk_pp/unexpected_char.hpp"
#include "diagnostics/variants/tk_pp/unexpected_else.hpp"
#include "diagnostics/variants/tk_pp/unknown_escape_seq.hpp"
#include "diagnostics/variants/tk_pp/untermed_char.h"
#include "diagnostics/variants/tk_pp/untermed_string.h"
#include "diagnostics/variants/tk_pp/utf8_too_many_chars.hpp"
#include "diagnostics/variants/todo.hpp"
#include "diagnostics/variants/unexpected_eof.hpp"

namespace jcc::diagnostics {
    constexpr auto c_ErrorColor{mjolnir::colors::light_red};

    [[nodiscard]]
    mjolnir::Report StartReport(DiagnosticData const &diag) {
        return {diag.m_ReportKind, diag.m_Source->m_Source, diag.m_StartPos};
    }

    void MjolnirVisitor::Print(Utf8TooManyChars const &diag) const {
        StartReport(diag)
                .with_message(
                        "A UTF-8, UTF-16, or UTF-32 character literal must "
                        "contain exactly one character."
                )
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(UntermedString const &diag) const {
        StartReport(diag)
                .with_message("Unterminated string constant.")
                .with_label(
                        mjolnir::Label{
                                {diag.m_Span.end() - 1, diag.m_Span.end()}
                        }.with_message("Reached end of line")
                                .with_color(c_ErrorColor)
                )
                .with_help(
                        "If you intended to write a multi-line string, "
                        "consider "
                        "escaping the newline character."
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(UntermedChar const &diag) const {
        StartReport(diag)
                .with_message("Unterminated character constant.")
                .with_label(
                        mjolnir::Label{
                                {diag.m_Span.end() - 1, diag.m_Span.end()}
                        }.with_message("Reached end of line")
                                .with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(UnknownEscapeSeq const &diag) const {
        StartReport(diag)
                .with_message("Unknown escape sequence.")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(PpConditionalNotTerminated const &diag) const {
        StartReport(diag)
                .with_message("Preprocessor conditional was not terminated")
                .with_label(
                        mjolnir::Label{diag.m_ConditionalSpan}
                                .with_message("Conditional starts here")
                                .with_color(mjolnir::colors::light_cyan)
                )
                .with_label(
                        mjolnir::Label{diag.m_EofSpan}
                                .with_message("End of file")
                                .with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(PpConditionalExpectedIdent const &diag) const {
        StartReport(diag)
                .with_message("Preprocessor conditional expected identifier.")
                .with_label(
                        mjolnir::Label{diag.m_ConditionalSpan}.with_color(
                                mjolnir::colors::light_cyan
                        )
                )
                .with_label(
                        mjolnir::Label{diag.m_UnexpectedTokenSpan}
                                .with_message(
                                        "Unexpected token, expected identifier"
                                )
                                .with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(OrphanedEndif const &diag) const {
        StartReport(diag)
                .with_message("Orphaned #endif directive.")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(PpNumberInvalid const &diag) const {
        StartReport(diag)
                .with_message("Invalid preprocessing number.")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(TodoError const &diag) const {
        StartReport(diag)
                .with_message("Compiler feature unimplemented.")
                .with_label(mjolnir::Label{diag.m_Span})
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(MultiByteCharImplDef const &diag) const {
        StartReport(diag)
                .with_message(
                        "The values of multi-character character constants "
                        "are implementation-defined."
                )
                .with_note(
                        "This implementation defines the behavior to be an "
                        "integer value "
                        "equal to the concatenation of the character values."
                )
                .with_label(
                        mjolnir::Label{diag.m_Span}
                                .with_color(mjolnir::colors::light_magenta)
                                .with_message(
                                        "Perhaps you meant to use a string "
                                        "literal?"
                                )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(IllegalMacroRedef const &diag) const {
        std::string                   message{"A macro may not be redefined."};
        std::optional<mjolnir::Label> defLabel{};
        mjolnir::BasicReportKind      kind{mjolnir::BasicReportKind::Error};

        if (diag.m_DefSpan.m_Source == diag.m_RedefineSpan.m_Source) {
            defLabel = mjolnir::Label{diag.m_DefSpan.m_Span}
                               .with_message("First defined here")
                               .with_color(mjolnir::colors::light_cyan);
        } else {
            kind = mjolnir::BasicReportKind::Continuation;
            mjolnir::Report{
                    mjolnir::BasicReportKind::Error,
                    diag.m_DefSpan.m_Source->m_Source,
                    diag.m_DefSpan.m_Span.start()
            }
                    .with_message(std::move(message))
                    .with_label(
                            mjolnir::Label{diag.m_DefSpan.m_Span}
                                    .with_message("First defined here")
                                    .with_color(mjolnir::colors::light_cyan)
                    )
                    .print(GetOstream());
        }

        mjolnir::Report report{
                kind, diag.m_RedefineSpan.m_Source->m_Source,
                diag.m_RedefineSpan.m_Span.start()
        };
        report.with_message(std::move(message))
                .with_label(
                        mjolnir::Label{diag.m_RedefineSpan.m_Span}
                                .with_message("Redefinition here")
                                .with_color(c_ErrorColor)
                );

        if (defLabel.has_value()) {
            report.with_label(defLabel.value());
        }

        report.print(GetOstream());
    }

    void MjolnirVisitor::Print(HexEscapeEmpty const &diag) const {
        StartReport(diag)
                .with_message("Hexadecimal escape sequence is empty")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(EscapeWithoutNewline const &diag) const {
        StartReport(diag)
                .with_message("Newline escape sequence without a newline.")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(EscapeSeqTooLarge const &diag) const {
        StartReport(diag)
                .with_message("Escape sequence value is too large")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(DiagnosticDirectiveNoTokens const &diag) const {
        mjolnir::Report report{StartReport(diag)};

        if (diag.m_Kind == DiagnosticDirectiveKind::Error) {
            report.with_message("Error directive without tokens");
        } else {
            report.with_message("Warning directive without tokens");
        }

        report.with_label(mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor))
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(CustomDiagnostic const &diag) const {
        StartReport(diag)
                .with_message(diag.m_Message)
                .with_label(mjolnir::Label{diag.m_Span})
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(CharConstGt4Chars const &diag) const {
        StartReport(diag)
                .with_message(
                        "Character constant contains more than 4 characters."
                )
                .with_label(
                        mjolnir::Label{diag.m_Span}
                                .with_message(
                                        "Perhaps you meant to use a string "
                                        "literal?"
                                )
                                .with_color(mjolnir::colors::light_magenta)
                )
                .with_note(
                        "Multi-character character constant values are "
                        "implementation defined."
                )
                .with_note(
                        "The value of a multi-character character constant "
                        "whose length is less than or equal to 4 is the "
                        "integer value equal to the concatenation of the "
                        "character values."
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(CharConstEmpty const &diag) const {
        mjolnir::Report report{StartReport(diag)};

        report.with_message("Character constant is empty.");

        if (diag.m_PotentiallyClosingQuote.has_value()) {
            report.with_label(
                          mjolnir::Label{diag.m_Span}
                                  .with_color(c_ErrorColor)
                                  .with_message(
                                          "This is the effective character "
                                          "constant, which is empty."
                                  )
            )
                    .with_label(
                            mjolnir::Label{diag.m_PotentiallyClosingQuote.value(
                                           )}
                                    .with_message(
                                            "This looks like it was meant to "
                                            "be "
                                            "the closing quote."
                                    )
                                    .with_color(mjolnir::colors::light_green)
                    )
                    .with_help("Perhaps you meant to write '\\''?");
        } else {
            report.with_label(
                    mjolnir::Label{diag.m_Span}
                            .with_color(c_ErrorColor)
                            .with_message("This character constant is empty.")
            );
        }

        report.print(GetOstream());
    }

    void MjolnirVisitor::Print(ShiftOperandNonInt const &diag) const {
        constexpr auto lhsColor{mjolnir::colors::light_cyan};
        constexpr auto rhsColor{mjolnir::colors::light_magenta};

        StartReport(diag)
                .with_message(
                        "Both operands in a shift expression must be of "
                        "integer types."
                )
                .with_label(mjolnir::Label{diag.m_OpSpan})
                .with_label(
                        mjolnir::Label{diag.m_LhsSpan}
                                .with_message(
                                        std::format(
                                                "This is of type {}",
                                                lhsColor.fg(diag.m_LhsType
                                                                    .ToString())
                                        )
                                )
                                .with_color(lhsColor)
                )
                .with_label(
                        mjolnir::Label{diag.m_RhsSpan}
                                .with_message(
                                        std::format(
                                                "This is of type {}",
                                                rhsColor.fg(diag.m_RhsType
                                                                    .ToString())
                                        )
                                )
                                .with_color(rhsColor)
                )
                .with_help(
                        "Consider casting the operand(s) to integer type(s)."
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(MultNonArithmetic const &diag) const {
        constexpr auto lhsColor{mjolnir::colors::light_cyan};
        constexpr auto rhsColor{mjolnir::colors::light_magenta};

        StartReport(diag)
                .with_message(
                        "The operands of a multiplicative expression must be "
                        "of "
                        "an arithmetic type."
                )
                .with_label(mjolnir::Label{diag.m_OpSpan})
                .with_label(
                        mjolnir::Label{diag.m_LhsSpan}
                                .with_message(
                                        std::format(
                                                "This is of type {}",
                                                lhsColor.fg(diag.m_LhsType
                                                                    .ToString())
                                        )
                                )
                                .with_color(lhsColor)
                )
                .with_label(
                        mjolnir::Label{diag.m_RhsSpan}
                                .with_message(
                                        std::format(
                                                "This is of type {}",
                                                rhsColor.fg(diag.m_RhsType
                                                                    .ToString())
                                        )
                                )
                                .with_color(rhsColor)
                )
                .with_help(
                        "Consider casting the operand(s) to arithmetic type(s)."
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(ModuloWithNonInt const &diag) const {
        constexpr auto lhsColor{mjolnir::colors::light_cyan};
        constexpr auto rhsColor{mjolnir::colors::light_magenta};

        StartReport(diag)
                .with_message(
                        "Both operands in a modulo expression must be of "
                        "integer types."
                )
                .with_label(mjolnir::Label{diag.m_OpSpan})
                .with_label(
                        mjolnir::Label{diag.m_LhsSpan}
                                .with_message(
                                        std::format(
                                                "This is of type {}",
                                                lhsColor.fg(diag.m_LhsType
                                                                    .ToString())
                                        )
                                )
                                .with_color(lhsColor)
                )
                .with_label(
                        mjolnir::Label{diag.m_RhsSpan}
                                .with_message(
                                        std::format(
                                                "This is of type {}",
                                                rhsColor.fg(diag.m_RhsType
                                                                    .ToString())
                                        )
                                )
                                .with_color(rhsColor)
                )
                .with_help(
                        "Consider casting the operand(s) to integer type(s)."
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(ExpectedExpression const &diag) const {
        auto report{StartReport(diag)};
        report.with_message("Expected expression to follow operator.");

        report.with_label(
                mjolnir::Label{diag.m_LhsSpan}
                        .with_message("Left-hand side")
                        .with_color(mjolnir::colors::light_blue)
        );
        if (diag.m_RhsSpan.has_value()) {
            report.with_label(
                          mjolnir::Label{diag.m_RhsSpan.value()}
                                  .with_message("Expected expression here")
                                  .with_color(c_ErrorColor)
            )
                    .with_label(
                            mjolnir::Label{diag.m_OpSpan}.with_color(
                                    mjolnir::colors::light_cyan
                            )
                    );
        } else {
            report.with_label(
                    mjolnir::Label{diag.m_OpSpan}.with_message(
                            "Expected this to be followed by an expression"
                    )
            );
        }

        report.print(GetOstream());
    }

    void MjolnirVisitor::Print(UnexpectedEof const &diag) const {
        StartReport(diag)
                .with_message("Unexpected EOF.")
                .with_label(mjolnir::Label{diag.m_Span})
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(UnexpectedChar const &diag) const {
        StartReport(diag)
                .with_message("Unexpected character.")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(IncludeOpenFailed const &diag) const {
        StartReport(diag)
                .with_message("Failed to open included file.")
                .with_label(mjolnir::Label{diag.m_DirectiveSpan})
                .with_label(
                        mjolnir::Label{diag.m_HeaderNameSpan}.with_color(
                                c_ErrorColor
                        )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(IncludeExpectedHeaderName const &diag) const {
        StartReport(diag)
                .with_message("Expected header name in #include directive.")
                .with_label(mjolnir::Label{diag.m_DirectiveSpan})
                .with_label(
                        mjolnir::Label{diag.m_HeaderNameSpan}.with_color(
                                c_ErrorColor
                        )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(DirectiveExpectedNewline const &diag) const {
        StartReport(diag)
                .with_message("Expected directive to be followed by a newline.")
                .with_label(mjolnir::Label{diag.m_DirectiveSpan})
                .with_label(
                        mjolnir::Label{diag.m_NextTokenSpan}
                                .with_color(c_ErrorColor)
                                .with_message("Expected newline here")
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(UndefExpectedIdent const &diag) const {
        StartReport(diag)
                .with_message("Undef directive expected identifier.")
                .with_label(mjolnir::Label{diag.m_DirectiveSpan})
                .with_label(
                        mjolnir::Label{diag.m_NextTokenSpan}.with_color(
                                c_ErrorColor
                        )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(InvalidMacroParam const &diag) const {
        StartReport(diag)
                .with_message("Unexpected token in macro parameter list.")
                .with_label(mjolnir::Label{diag.m_MacroDefSpan})
                .with_label(
                        mjolnir::Label{diag.m_InvalidParamSpan}
                                .with_color(c_ErrorColor)
                                .with_message(
                                        "Expected either an ellipsis or an "
                                        "identifier"
                                )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(MacroNameNotIdent const &diag) const {
        StartReport(diag)
                .with_message("Expected identifier for macro name.")
                .with_label(mjolnir::Label{diag.m_DirectiveSpan})
                .with_label(
                        mjolnir::Label{diag.m_MacroNameSpan}.with_color(
                                c_ErrorColor
                        )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(UnexpectedElse const &diag) const {
        StartReport(diag)
                .with_message(
                        "Cannot use an else-type directive after an else "
                        "directive."
                )
                .with_label(
                        mjolnir::Label{diag.m_ElseSpan}
                                .with_color(mjolnir::colors::light_cyan)
                                .with_message("Valid else directive used here")
                )
                .with_label(
                        mjolnir::Label{diag.m_UnexpectedElseSpan}
                                .with_color(c_ErrorColor)
                                .with_message(
                                        "Followed up by unexpected else "
                                        "directive here"
                                )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(ElseWithoutIf const &diag) const {
        StartReport(diag)
                .with_message("#else without #if")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(MacroExpectedCommaOrRparen const &diag) const {
        StartReport(diag)
                .with_message(
                        "Expected comma or right parenthesis in macro "
                        "parameter list."
                )
                .with_label(mjolnir::Label{diag.m_MacroSpan})
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(MacroEllipsisNotLast const &diag) const {
        StartReport(diag)
                .with_message(
                        "The ellipsis must be the last parameter in a variadic "
                        "macro."
                )
                .with_label(mjolnir::Label{diag.m_MacroSpan})
                .with_label(
                        mjolnir::Label{diag.m_CommaSpan}.with_color(
                                c_ErrorColor
                        )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(MacroInvocExpectedLparen const &diag) const {
        StartReport(diag)
                .with_message(
                        "Left parenthesis expected after function-like macro "
                        "invocation."
                )
                .with_label(mjolnir::Label{diag.m_MacroNameSpan})
                .with_label(
                        mjolnir::Label{diag.m_NextTokenSpan}.with_color(
                                c_ErrorColor
                        )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(MacroInvocInvalidNumArgs const &diag) const {
        auto const expectedArgsStr{std::to_string(diag.m_ExpectedNArgs)};
        auto const receivedArgsStr{std::to_string(diag.m_ReceivedNArgs)};

        StartReport(diag)
                .with_message(
                        std::format(
                                "Expected {}{} arguments, but received {}.",
                                diag.m_IsMinimum ? "at least " : "",
                                mjolnir::colors::light_green.fg(
                                        expectedArgsStr
                                ),
                                c_ErrorColor.fg(receivedArgsStr)
                        )
                )
                .with_label(mjolnir::Label{diag.m_MacroInvocSpan})
                .with_label(
                        mjolnir::Label{diag.m_MacroArgsSpan}.with_color(
                                c_ErrorColor
                        )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(UnrecognizedIntegerSuffix const &diag) const {
        StartReport(diag)
                .with_message("Unrecognized integer suffix.")
                .with_label(
                        mjolnir::Label{diag.m_SuffixSpan}.with_color(
                                c_ErrorColor
                        )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(UnrecognizedFloatingSuffix const &diag) const {
        StartReport(diag)
                .with_message("Unrecognized floating point suffix.")
                .with_label(
                        mjolnir::Label{diag.m_SuffixSpan}.with_color(
                                c_ErrorColor
                        )
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(InvalidFloatingPointLiteral const &diag) const {
        StartReport(diag)
                .with_message("Invalid floating point literal.")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(InvalidIntegerLiteral const &diag) const {
        StartReport(diag)
                .with_message("Invalid integer literal.")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void MjolnirVisitor::Print(BasicSyntaxError const &diag) const {
        auto report = StartReport(diag).with_label(
                mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
        );

        if (std::holds_alternative<std::string>(diag.m_Message)) {
            auto &message{std::get<std::string>(diag.m_Message)};

            report.with_message(std::move(message));
        } else if (std::holds_alternative<
                           BasicSyntaxError::ExpectedReceivedPair>(
                           diag.m_Message
                   )) {
            auto &[expected,
                   received]{std::get<BasicSyntaxError::ExpectedReceivedPair>(
                    diag.m_Message
            )};
            constexpr auto expectedColor{mjolnir::colors::light_green};
            constexpr auto receivedColor{c_ErrorColor};

            report.with_message(
                    std::format(
                            "Expected {} but received {}.",
                            expectedColor.fg(expected),
                            receivedColor.fg(received)
                    )
            );
        }

        report.print(GetOstream());
    }

    void MjolnirVisitor::Print(NoCompatIntType const &diag) const {
        StartReport(diag)
                .with_message("No compatible integer type found.")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }

    void
    MjolnirVisitor::Print(InvalidSpecifierQualifierList const &diag) const {
        StartReport(diag)
                .with_message("Invalid specifier-qualifier list.")
                .with_label(
                        mjolnir::Label{diag.m_Span}.with_color(c_ErrorColor)
                )
                .print(GetOstream());
    }
}// namespace jcc::diagnostics
