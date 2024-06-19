#ifndef MACRO_H
#define MACRO_H

#include <vector>
#include "../Tokenizer.h"

namespace jcc::preprocessor::macro {
	struct ReplacementList {
		using TokenList = std::vector<Tokenizer::Token>;
		TokenList m_ReplacementList{};

		[[nodiscard]]
		bool
		operator==(ReplacementList const &other) const noexcept;
	};

	struct ObjectLikeMacro final {
		std::string m_MacroName{};
		ReplacementList m_ReplacementList{};

		[[nodiscard]]
		bool
		operator==(ObjectLikeMacro const &other) const noexcept;
	};

	struct FunctionLikeMacro final {
		using ParameterList = std::vector<Tokenizer::Identifier>;

		std::string m_MacroName{};
		ReplacementList m_ReplacementList{};
		ParameterList m_ParameterList{};
		bool m_IsVA{};

		[[nodiscard]]
		bool
		operator==(FunctionLikeMacro const &other) const noexcept;
	};

	using Macro = std::variant<ObjectLikeMacro, FunctionLikeMacro>;
}

#endif //MACRO_H
