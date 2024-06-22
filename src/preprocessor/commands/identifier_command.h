#ifndef IDENTIFIER_COMMAND_H
#define IDENTIFIER_COMMAND_H

#include "command.h"
#include "preprocessor/macro.h"

namespace jcc::preprocessor {
	class Preprocessor;
}

namespace jcc::preprocessor::commands {
	class IdentifierCommand final : public Command {
		[[nodiscard]]
		static macro::FnMacroArguments
		GatherArguments(Preprocessor &preprocessor, macro::FunctionLikeMacro const &fnMacro);

		[[nodiscard]]
		static std::pair<bool, std::vector<Tokenizer::Token>> GatherArgumentTokens(Preprocessor &preprocessor);

	public:
		explicit IdentifierCommand(CommandMap &map);

		~IdentifierCommand() override;

		[[nodiscard]]
		std::optional<PreprocessorToken> Execute(Preprocessor &preprocessor, Tokenizer::Token &&ident) const override;
	};
}// namespace jcc::preprocessor::commands

#endif//IDENTIFIER_COMMAND_H
