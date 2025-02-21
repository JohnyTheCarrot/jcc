#ifndef CHAR_CONST_EMPTY_H
#define CHAR_CONST_EMPTY_H

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct CharConstEmpty final : DiagnosticData {
        mjolnir::Span                m_Span;
        std::optional<mjolnir::Span> m_PotentiallyClosingQuote;

        CharConstEmpty(
                std::shared_ptr<Source> source, mjolnir::Span span,
                std::optional<mjolnir::Span> const &potentiallyClosingQuote
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//CHAR_CONST_EMPTY_H
