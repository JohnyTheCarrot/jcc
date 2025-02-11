#ifndef TODO_HPP
#define TODO_HPP

#include <mjolnir/span.hpp>

#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class TodoError final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        TodoError(std::shared_ptr<Source> source, mjolnir::Span span);

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//TODO_HPP
