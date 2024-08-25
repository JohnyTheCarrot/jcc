#include "macro.h"

namespace jcc::preprocessor::macro {
	std::ostream &operator<<(std::ostream &os, FnMacroArguments const &args) {
		for (auto const &[name, value]: args) {
			os << testing::PrintToString(name) << " -> ";
			os << testing::PrintToString(value) << '\n';
		}

		return os;
	}
}// namespace jcc::preprocessor::macro
