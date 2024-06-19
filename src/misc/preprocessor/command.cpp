#include "command.h"

namespace jcc::preprocessor {
	Command::Command(CommandMap &map, Tokenizer::Token::Type tokenType) {
		map.emplace(tokenType, this);
	}

	PreprocessorCommandSingleton::PreprocessorCommandSingleton() {
		m_Commands.emplace_back(std::make_unique<DefineCommand>(m_CommandMap));
	}

	CommandMap const &PreprocessorCommandSingleton::GetCommandMap() const noexcept {
		return m_CommandMap;
	}
}// namespace jcc::preprocessor
