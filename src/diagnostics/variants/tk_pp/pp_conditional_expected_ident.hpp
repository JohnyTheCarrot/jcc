#ifndef PP_CONDITIONAL_EXPECTED_IDENT_HPP
#define PP_CONDITIONAL_EXPECTED_IDENT_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class PpConditionalExpectedIdent final : public DiagnosticData {
        mjolnir::Span m_ConditionalSpan;
        mjolnir::Span m_UnexpectedTokenSpan;

    public:
        PpConditionalExpectedIdent(
                std::shared_ptr<Source> source, mjolnir::Span condSpan,
                mjolnir::Span unexpectedTokenSpan
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//PP_CONDITIONAL_EXPECTED_IDENT_HPP
