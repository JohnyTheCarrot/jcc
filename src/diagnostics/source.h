#ifndef SOURCE_H
#define SOURCE_H

#include <mjolnir/source.hpp>
#include <string>

namespace jcc::diagnostics {
    // This class serves to make sure the lifetime of the buffer is longer than the lifetime of the Mjölnir source.
    struct Source final {
        std::string     m_Buffer;
        mjolnir::Source m_Source;

        Source(std::string &&buffer, std::string const &fileName);
    };
}// namespace jcc::diagnostics

#endif//SOURCE_H
