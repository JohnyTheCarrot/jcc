#ifndef UNTERMED_CHAR_H
#define UNTERMED_CHAR_H

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class UntermedChar final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        UntermedChar(std::shared_ptr<Source> const &source, mjolnir::Span span);

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//UNTERMED_CHAR_H
