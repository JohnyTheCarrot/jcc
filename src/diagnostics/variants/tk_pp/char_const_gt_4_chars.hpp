#ifndef CHAR_CONST_GT_4_CHARS_HPP
#define CHAR_CONST_GT_4_CHARS_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct CharConstGt4Chars final : DiagnosticData {
        mjolnir::Span m_Span;

        CharConstGt4Chars(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//CHAR_CONST_GT_4_CHARS_HPP
