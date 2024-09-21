#include "macro_store.h"

namespace jcc::preprocessor {
	MacroStore::MacroStore() = default;

	bool MacroStore::IsMacroDefined(std::string const &name) const {
		return m_MacroDefinitions.contains(name);
	}

	void MacroStore::RegisterMacro(std::string const &name, macro::Macro macro) {
		m_MacroDefinitions.emplace(name, std::move(macro));
	}

	void MacroStore::InvokeMacro(macro::MacroInvocation &&macroInvocation) {
		// using macro::operator<<;
		// std::cout << "Invoking macro: " << macroInvocation.m_MacroName << '\n';
		//
		// if (macroInvocation.m_Args.has_value())
		// 	std::cout << "Arguments:\n" << macroInvocation.m_Args.value() << '\n';

		m_MacroStack.push(macroInvocation);
	}

	macro::Macro const *MacroStore::GetMacro(std::string const &name, bool allowCurrentMacro) const {
		if (auto const macroIt{std::ranges::find_if(
		            m_MacroDefinitions,
		            [&](auto &pair) {
			            if (!m_MacroStack.empty()) {
				            auto const &topInvokedMacro{m_MacroStack.top()};
				            if (pair.first == topInvokedMacro.m_MacroName)
					            return allowCurrentMacro;
			            }

			            return pair.first == name;
		            }
		    )};
		    macroIt != m_MacroDefinitions.end()) {
			return &macroIt->second;
		}

		return nullptr;
	}

	std::optional<std::vector<tokenizer::Token>> MacroStore::GetMacroArgument(std::string const &argumentName) const {
		if (m_MacroStack.empty())
			return std::nullopt;

		auto const macroTop{m_MacroStack.top()};
		if (!macroTop.m_Args.has_value())
			return std::nullopt;

		auto const argIt{std::ranges::find_if(macroTop.m_Args.value(), [&](auto const &arg) {
			return arg.first == argumentName;
		})};

		if (argIt == macroTop.m_Args.value().end())
			return std::nullopt;

		return argIt->second;
	}

	std::optional<tokenizer::Token> MacroStore::GetTokenFromMacroStack() {
		if (m_MacroStack.empty())
			return std::nullopt;

		auto &[macroName, currentTokenIndex, args, currentArgReader]{m_MacroStack.top()};
		++currentTokenIndex;

		if (auto const *macro{GetMacro(macroName, true)}; macro != nullptr) {
			auto const [m_ReplacementList]{[macro] {
				if (std::holds_alternative<macro::ObjectLikeMacro>(*macro)) {
					return std::get<macro::ObjectLikeMacro>(*macro).m_ReplacementList;
				}

				return std::get<macro::FunctionLikeMacro>(*macro).m_ReplacementList;
			}()};

			if (currentTokenIndex >= static_cast<int>(m_ReplacementList.size())) {
				m_MacroStack.pop();
				return GetTokenFromMacroStack();
			}

			return m_ReplacementList.at(currentTokenIndex);
		}

		throw std::runtime_error{"Macro stack is not empty, but the macro is not found"};
	}

	std::optional<tokenizer::Token> MacroStore::GetTokenFromMacroArgumentReader() {
		if (m_MacroStack.empty())
			return std::nullopt;

		auto &macroArgReader{m_MacroStack.top().m_CurrentArgReader};

		if (!macroArgReader.has_value())
			return std::nullopt;

		auto &[m_Args, m_CurrentTokenIndex]{macroArgReader.value()};

		++m_CurrentTokenIndex;

		if (m_CurrentTokenIndex >= static_cast<int>(m_Args.size())) {
			macroArgReader = std::nullopt;
			return std::nullopt;
		}

		return m_Args.at(m_CurrentTokenIndex);
	}

	void MacroStore::PushMacroArgumentTokens(std::vector<tokenizer::Token> &&args) {
		if (m_MacroStack.empty())
			throw std::runtime_error{"Macro argument reader is set, but the macro stack is empty"};

		auto &currentMacro{m_MacroStack.top()};
		currentMacro.m_CurrentArgReader = macro::MacroArgumentReader{std::move(args)};
	}
}// namespace jcc::preprocessor
