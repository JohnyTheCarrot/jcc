#ifndef PP_NUMBER_INVALID_HPP
#define PP_NUMBER_INVALID_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class PpNumberInvalid final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        PpNumberInvalid(std::shared_ptr<Source> source, mjolnir::Span span);

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//PP_NUMBER_INVALID_HPP
