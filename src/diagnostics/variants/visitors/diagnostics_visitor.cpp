#include "diagnostics_visitor.hpp"

namespace jcc::diagnostics {
    std::ostream &DiagnosticsVisitor::GetOstream() const {
        return *m_Ostream;
    }

    DiagnosticsVisitor::DiagnosticsVisitor(std::ostream &ostream)
        : m_Ostream{&ostream} {
    }
}// namespace jcc::diagnostics
