#ifndef ESCAPE_WITHOUT_NEWLINE_HPP
#define ESCAPE_WITHOUT_NEWLINE_HPP

#include <diagnostics/diagnostics.h>
#include <mjolnir/span.hpp>

namespace jcc::diagnostics {
    class EscapeWithoutNewline final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        EscapeWithoutNewline(
                std::shared_ptr<Source> source, mjolnir::Span span
        );

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//ESCAPE_WITHOUT_NEWLINE_HPP
