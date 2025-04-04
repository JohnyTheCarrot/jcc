#ifndef CHAR_CONST_GT_4_CHARS_HPP
#define CHAR_CONST_GT_4_CHARS_HPP

#include <memory>          // for shared_ptr
#include <mjolnir/span.hpp>// for Span

#include "diagnostics/diagnostics.h"// for DiagnosticData

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    struct CharConstGt4Chars final : DiagnosticData {
        mjolnir::Span m_Span;

        CharConstGt4Chars(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//CHAR_CONST_GT_4_CHARS_HPP
