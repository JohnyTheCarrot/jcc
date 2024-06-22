#include "preprocessor_token.h"

namespace jcc::preprocessor {
	bool PreprocessorToken::operator==(PreprocessorToken const &other) const noexcept {
		return m_Token == other.m_Token && m_IsFromMacro == other.m_IsFromMacro;
	}
}// namespace jcc::preprocessor
