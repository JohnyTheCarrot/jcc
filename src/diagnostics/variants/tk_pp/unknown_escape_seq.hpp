#ifndef UNKNOWN_ESCAPE_SEQ_HPP
#define UNKNOWN_ESCAPE_SEQ_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class UnknownEscapeSeq final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        UnknownEscapeSeq(std::shared_ptr<Source> source, mjolnir::Span span);

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//UNKNOWN_ESCAPE_SEQ_HPP
