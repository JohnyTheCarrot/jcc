#ifndef BASIC_SYNTAX_ERROR_HPP
#define BASIC_SYNTAX_ERROR_HPP

#include <string>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct BasicSyntaxError final : DiagnosticData {
        using ExpectedReceivedPair = std::pair<std::string, std::string>;

        mjolnir::Span                                   m_Span;
        std::variant<std::string, ExpectedReceivedPair> m_Message;

        BasicSyntaxError(
                std::shared_ptr<Source> source, mjolnir::Span span,
                std::string message
        );

        BasicSyntaxError(
                std::shared_ptr<Source> source, mjolnir::Span span,
                std::string_view expected, std::string_view received
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//BASIC_SYNTAX_ERROR_HPP
