#ifndef CHAR_CONST_GT_4_CHARS_HPP
#define CHAR_CONST_GT_4_CHARS_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class CharConstGt4Chars final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        CharConstGt4Chars(std::shared_ptr<Source> source, mjolnir::Span span);

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//CHAR_CONST_GT_4_CHARS_HPP
