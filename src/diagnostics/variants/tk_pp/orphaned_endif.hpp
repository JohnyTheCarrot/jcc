#ifndef ORPHANED_ENDIF_HPP
#define ORPHANED_ENDIF_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    struct OrphanedEndif final : DiagnosticData {
        mjolnir::Span m_Span;

        OrphanedEndif(std::shared_ptr<Source> source, mjolnir::Span span);

        void Visit(DiagnosticsVisitor const &visitor) const override;
    };
}// namespace jcc::diagnostics

#endif//ORPHANED_ENDIF_HPP
