#ifndef UNTERMED_STRING_H
#define UNTERMED_STRING_H

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class UntermedString final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        UntermedString(
                std::shared_ptr<Source> const &source, mjolnir::Span span
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//UNTERMED_STRING_H
