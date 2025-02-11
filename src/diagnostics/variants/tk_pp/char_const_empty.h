#ifndef CHAR_CONST_EMPTY_H
#define CHAR_CONST_EMPTY_H

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class CharConstEmpty final : public DiagnosticData {
        mjolnir::Span                m_Span;
        std::optional<mjolnir::Span> m_PotentiallyClosingQuote;

    public:
        CharConstEmpty(
                std::shared_ptr<Source> const &source, mjolnir::Span span,
                std::optional<mjolnir::Span> potentiallyClosingQuote
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//CHAR_CONST_EMPTY_H
