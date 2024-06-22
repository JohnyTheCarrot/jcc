#ifndef PREPROCESSOR_TOKEN_H
#define PREPROCESSOR_TOKEN_H

#include "tokenizer/Tokenizer.h"

namespace jcc::preprocessor {
	struct PreprocessorToken final {
		Tokenizer::Token m_Token;
		bool             m_IsFromMacro{false};

		[[nodiscard]]
		bool
		operator==(PreprocessorToken const &other) const noexcept;
	};
}// namespace jcc::preprocessor

#endif//PREPROCESSOR_TOKEN_H
