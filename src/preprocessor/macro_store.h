#ifndef MACRO_STORE_H
#define MACRO_STORE_H

#include "macro.h"

#include <string>
#include <unordered_map>

namespace jcc::preprocessor {
	class MacroStore final {
		using MacroDefinitions = std::unordered_map<std::string, macro::Macro>;

		MacroDefinitions                   m_MacroDefinitions{};
		std::stack<macro::MacroInvocation> m_MacroStack{};

	public:
		MacroStore();

		[[nodiscard]]
		bool IsMacroDefined(std::string const &name) const;

		void RegisterMacro(std::string const &name, macro::Macro macro);

		void InvokeMacro(macro::MacroInvocation &&macroInvocation);

		[[nodiscard]]
		macro::Macro const *GetMacro(std::string const &name, bool allowCurrentMacro = false) const;

		[[nodiscard]]
		std::optional<std::vector<tokenizer::Token>> GetMacroArgument(std::string const &argumentName) const;

		[[nodiscard]]
		std::optional<tokenizer::Token> GetTokenFromMacroStack();

		[[nodiscard]]
		std::optional<tokenizer::Token> GetTokenFromMacroArgumentReader();

		void PushMacroArgumentTokens(std::vector<tokenizer::Token> &&args);
	};
}// namespace jcc::preprocessor

#endif//MACRO_STORE_H
