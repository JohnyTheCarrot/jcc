#ifndef ORPHANED_ENDIF_HPP
#define ORPHANED_ENDIF_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class OrphanedEndif final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        OrphanedEndif(std::shared_ptr<Source> source, mjolnir::Span span);

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//ORPHANED_ENDIF_HPP
