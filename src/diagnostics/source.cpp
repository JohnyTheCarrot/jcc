#include "source.h"

namespace jcc::diagnostics {
    Source::Source(std::string &&buffer, std::string const &fileName)
        : m_Buffer{std::move(buffer)}
        , m_Source{fileName, m_Buffer} {
    }
}// namespace jcc::diagnostics
