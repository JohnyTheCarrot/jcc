#ifndef SOURCE_H
#define SOURCE_H

#include <mjolnir/source.hpp>
#include <string>

namespace jcc::diagnostics {
    struct Source final {
        std::string     m_Buffer;
        mjolnir::Source m_Source;

        Source(std::string &&buffer, std::string const &fileName);
    };
}// namespace jcc::diagnostics

#endif//SOURCE_H
