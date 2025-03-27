#ifndef UNRECOGNIZED_INTEGER_SUFFIX_HPP
#define UNRECOGNIZED_INTEGER_SUFFIX_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct UnrecognizedIntegerSuffix final : DiagnosticData {
        mjolnir::Span m_SuffixSpan;

        UnrecognizedIntegerSuffix(
                std::shared_ptr<Source> source, mjolnir::Span suffixSpan
        );

        void
        Visit(DiagnosticsVisitor const &visitor
        ) const override;
    };
}// namespace jcc::diagnostics

#endif//UNRECOGNIZED_INTEGER_SUFFIX_HPP
