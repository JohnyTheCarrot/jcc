#ifndef COMMAND_H
#define COMMAND_H

#include "../Tokenizer.h"
#include "macro.h"
#include "preprocessor.h"
#include <optional>
#include <unordered_map>

namespace jcc::preprocessor {
	using CommandMap = std::unordered_map<Tokenizer::Token::Type, class Command *>;

	class Preprocessor;

	class Command {
	public:
		Command(CommandMap &map, Tokenizer::Token::Type tokenType);

		virtual ~Command() = default;

		[[nodiscard]]
		virtual std::optional<Tokenizer::Token> Execute(Preprocessor &preprocessor) const = 0;
	};

	class DefineCommand final : public Command {
		[[nodiscard]]
		static macro::ReplacementList GatherReplacementList(Preprocessor &preprocessor);

		[[nodiscard]]
		/// \return a pair of whether the parameter list is variadic and the parameter list
		static std::pair<bool, macro::FunctionLikeMacro::ParameterList> GatherParameterList(Preprocessor &preprocessor);

		static void
		DefineObjectLikeMacro(Preprocessor &preprocessor, std::string &&name, Tokenizer::Token &&firstToken);

		static void DefineFunctionLikeMacro(Preprocessor &preprocessor, std::string &&name);

	public:
		explicit DefineCommand(CommandMap &map);

		~DefineCommand() override = default;

		[[nodiscard]]
		std::optional<Tokenizer::Token> Execute(Preprocessor &preprocessor) const override;
	};

	class PreprocessorCommandSingleton final {
		CommandMap                            m_CommandMap{};
		std::vector<std::unique_ptr<Command>> m_Commands{};

		PreprocessorCommandSingleton();

	public:
		static PreprocessorCommandSingleton &GetInstance();

		CommandMap const &GetCommandMap() const noexcept;
	};
}// namespace jcc::preprocessor

#endif// COMMAND_H
