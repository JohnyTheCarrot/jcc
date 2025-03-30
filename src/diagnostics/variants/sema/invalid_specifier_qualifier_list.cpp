#include "invalid_specifier_qualifier_list.hpp"

#include "diagnostics/variants/visitors/diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    InvalidSpecifierQualifierList::InvalidSpecifierQualifierList(
            std::shared_ptr<Source> source, mjolnir::Span span
    )
        : DiagnosticData{std::move(source), span.start(), mjolnir::BasicReportKind::Error}
        , m_Span{span} {
    }

    void InvalidSpecifierQualifierList::Visit(
            DiagnosticsVisitor const &visitor
    ) const {
        visitor.Print(*this);
    }
}// namespace jcc::diagnostics
