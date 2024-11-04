#include "macro.h"

#include <ostream>// for basic_ostream, operator<<, basic_ios, ostream
#include <utility>// for pair

#include "gtest/gtest.h"// for PrintToString

namespace jcc::preprocessor::macro {
    std::ostream &operator<<(std::ostream &os, FnMacroArguments const &args) {
        for (auto const &[name, value] : args) {
            os << testing::PrintToString(name) << " -> ";
            os << testing::PrintToString(value) << '\n';
        }

        return os;
    }
}// namespace jcc::preprocessor::macro
