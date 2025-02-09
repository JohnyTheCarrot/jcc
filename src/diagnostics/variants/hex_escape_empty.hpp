#ifndef HEX_CHAR_EMPTY_HPP
#define HEX_CHAR_EMPTY_HPP
#include "diagnostics/diagnostics.h"

namespace jcc::diagnostics {
    class HexEscapeEmpty final : public DiagnosticData {
        mjolnir::Span m_Span;

    public:
        HexEscapeEmpty(std::shared_ptr<Source> source, mjolnir::Span span);

        void Print(std::ostream &ostream) const override;
    };
}// namespace jcc::diagnostics

#endif//HEX_CHAR_EMPTY_HPP
