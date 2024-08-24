#ifndef UTILS_H
#define UTILS_H

#include "misc/compiler_data_types.h"
#include <optional>

namespace jcc {
	struct SpanMarker;
}

namespace jcc::tokenizer {
	enum class ConstantPrefix;
	class CharIter;
}// namespace jcc::tokenizer

namespace jcc::tokenizer::utils {
	enum class ConstantType {
		String,
		Character,
	};

	// for character constants and string literals
	[[nodiscard]]
	std::optional<compiler_data_types::Char32::type> ReadSingleCharacter(
	        CharIter &charIter, ConstantPrefix prefix, SpanMarker const &startMarker, ConstantType constantType
	);
}// namespace jcc::tokenizer::utils

#endif//UTILS_H
