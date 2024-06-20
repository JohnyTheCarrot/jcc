#ifndef COMMAND_H
#define COMMAND_H

#include "../../Tokenizer.h"
#include "../preprocessor.h"
#include <optional>
#include <unordered_map>

namespace jcc::preprocessor {
	class Preprocessor;

	namespace commands {
		using CommandMap = std::unordered_map<Tokenizer::Token::Type, class Command *>;

		class Command {
		public:
			Command(CommandMap &map, Tokenizer::Token::Type tokenType);

			virtual ~Command() = default;

			[[nodiscard]]
			virtual std::optional<Tokenizer::Token> Execute(Preprocessor &preprocessor, Tokenizer::Token &&) const = 0;
		};

		class PreprocessorCommandSingleton final {
			CommandMap                            m_CommandMap{};
			std::vector<std::unique_ptr<Command>> m_Commands{};

			PreprocessorCommandSingleton();

		public:
			static PreprocessorCommandSingleton &GetInstance();

			CommandMap const &GetCommandMap() const noexcept;
		};
	}// namespace commands
}// namespace jcc::preprocessor

#endif// COMMAND_H
