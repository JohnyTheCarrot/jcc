#ifndef UNDEF_EXPECTED_IDENT_HPP
#define UNDEF_EXPECTED_IDENT_HPP
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
    struct UndefExpectedIdent final : DiagnosticData {
        mjolnir::Span m_DirectiveSpan;
        mjolnir::Span m_NextTokenSpan;

        UndefExpectedIdent(
                std::shared_ptr<Source> source, mjolnir::Span directiveSpan,
                mjolnir::Span nextTokenSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UNDEF_EXPECTED_IDENT_HPP
