#ifndef MACRO_H
#define MACRO_H

#include "../Tokenizer.h"
#include <unordered_map>
#include <vector>

namespace jcc::preprocessor::macro {
	struct ReplacementList {
		using TokenList = std::vector<Tokenizer::Token>;
		TokenList m_ReplacementList{};

		[[nodiscard]]
		bool
		operator==(ReplacementList const &other) const noexcept;
	};

	struct ObjectLikeMacro final {
		std::string     m_MacroName{};
		ReplacementList m_ReplacementList{};

		[[nodiscard]]
		bool
		operator==(ObjectLikeMacro const &other) const noexcept;
	};

	struct FunctionLikeMacro final {
		using ParameterList = std::vector<Tokenizer::Identifier>;

		std::string     m_MacroName{};
		ReplacementList m_ReplacementList{};
		ParameterList   m_ParameterList{};
		bool            m_IsVA{};

		[[nodiscard]]
		bool
		operator==(FunctionLikeMacro const &other) const noexcept;
	};

	using Macro = std::variant<ObjectLikeMacro, FunctionLikeMacro>;

	using FnMacroArguments = std::unordered_map<std::string, std::vector<Tokenizer::Token>>;

	struct MacroArgumentReader final {
		std::vector<Tokenizer::Token> m_Args{};
		int                           m_CurrentTokenIndex{-1};
	};

	struct MacroInvocation final {
		std::string                        m_MacroName{};
		int                                m_CurrentToken{-1};
		std::optional<FnMacroArguments>    m_Args{};
		std::optional<MacroArgumentReader> m_CurrentArgReader{};
	};
}// namespace jcc::preprocessor::macro

#endif//MACRO_H
