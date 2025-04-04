#ifndef UNRECOGNIZED_FLOATING_SUFFIX_HPP
#define UNRECOGNIZED_FLOATING_SUFFIX_HPP

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
    struct UnrecognizedFloatingSuffix final : DiagnosticData {
        mjolnir::Span m_SuffixSpan;

        UnrecognizedFloatingSuffix(
                std::shared_ptr<Source> source, mjolnir::Span suffixSpan
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//UNRECOGNIZED_FLOATING_SUFFIX_HPP
