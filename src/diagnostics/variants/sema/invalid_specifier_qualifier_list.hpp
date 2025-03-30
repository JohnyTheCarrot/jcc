#ifndef INVALID_SPECIFIER_QUALIFIER_LIST_HPP
#define INVALID_SPECIFIER_QUALIFIER_LIST_HPP

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct InvalidSpecifierQualifierList final : DiagnosticData {
        mjolnir::Span m_Span;

        InvalidSpecifierQualifierList(
                std::shared_ptr<Source> source, mjolnir::Span span
        );

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//INVALID_SPECIFIER_QUALIFIER_LIST_HPP
