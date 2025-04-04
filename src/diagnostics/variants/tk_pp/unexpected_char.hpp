#ifndef UNEXPECTED_CHAR_HPP
#define UNEXPECTED_CHAR_HPP

#include <memory>// for shared_ptr

#include "diagnostics/diagnostics.h"// for DiagnosticData
#include "mjolnir/span.hpp"         // for Span

namespace jcc {
    namespace diagnostics {
        class DiagnosticsVisitor;
        struct Source;
    }// namespace diagnostics
}// namespace jcc

namespace jcc::diagnostics {
    struct UnexpectedChar final : DiagnosticData {
        mjolnir::Span m_Span;

        UnexpectedChar(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UNEXPECTED_CHAR_HPP
