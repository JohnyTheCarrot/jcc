#ifndef HEX_CHAR_EMPTY_HPP
#define HEX_CHAR_EMPTY_HPP
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
    struct HexEscapeEmpty final : public DiagnosticData {
        mjolnir::Span m_Span;

        HexEscapeEmpty(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//HEX_CHAR_EMPTY_HPP
