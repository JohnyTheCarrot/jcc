#ifndef JCC_PREPROCESSOR_H
#define JCC_PREPROCESSOR_H

#include "tokenizer/Tokenizer.h"
#include <fstream>
#include <stack>

namespace jcc {
	class Preprocessor final {
		struct ReplacementList {
			using TokenList = std::vector<Tokenizer::Token>;
			TokenList m_ReplacementList{};

			[[nodiscard]]
			bool
			operator==(ReplacementList const &other) const noexcept;
		};

		struct ObjectLikeMacro final {
			Tokenizer::Identifier::IdentString m_MacroName{};
			ReplacementList                    m_ReplacementList{};

			[[nodiscard]]
			bool
			operator==(ObjectLikeMacro const &other) const noexcept;
		};

		struct FunctionLikeMacro final {
			using ParameterList = std::vector<Tokenizer::Identifier>;

			Tokenizer::Identifier::IdentString m_MacroName{};
			ReplacementList                    m_ReplacementList{};
			ParameterList                      m_ParameterList{};
			bool                               m_IsVA{};

			[[nodiscard]]
			bool
			operator==(FunctionLikeMacro const &other) const noexcept;
		};

		using Macro = std::variant<ObjectLikeMacro, FunctionLikeMacro>;

		struct MacroReplacementStackData final {
			using ArgumentList = std::optional<std::vector<ReplacementList>>;

			Macro                                                     m_Macro{ObjectLikeMacro{}};
			ArgumentList                                              m_Arguments{};
			std::optional<ReplacementList::TokenList::const_iterator> m_ArgumentTokenIter{};
			std::optional<ReplacementList::TokenList::const_iterator> m_ArgumentTokenIterEnd{};
			std::optional<ObjectLikeMacro>                            m_VaArgsMacro{};
			ReplacementList::TokenList::const_iterator                m_Progress{};

			MacroReplacementStackData() = default;

			explicit MacroReplacementStackData(ObjectLikeMacro const &objectLikeMacro)
			    : m_Macro{objectLikeMacro}
			    , m_Progress{std::get<ObjectLikeMacro>(m_Macro).m_ReplacementList.m_ReplacementList.cbegin()} {
			}

			explicit MacroReplacementStackData(FunctionLikeMacro const &functionLikeMacro, ArgumentList &&argumentList)
			    : m_Macro{functionLikeMacro}
			    , m_Arguments{std::move(argumentList)}
			    , m_Progress{std::get<FunctionLikeMacro>(m_Macro).m_ReplacementList.m_ReplacementList.cbegin()} {
			}

			[[nodiscard]]
			bool IsFinished() const noexcept;

			[[nodiscard]]
			Tokenizer::Identifier::IdentString const &GetMacroName() const noexcept;
		};

		Tokenizer                                   m_MainTokenizer;
		std::vector<std::unique_ptr<std::ifstream>> m_IStreams{};
		std::stack<std::unique_ptr<Tokenizer>>      m_TokenizerStack{};
		Tokenizer                                  *m_CurrentTokenizer;
		Diagnosis::Vec                             &m_Diagnoses;
		std::stack<MacroReplacementStackData>       m_MacroStack{};
		std::optional<Tokenizer::Token>             m_UnusedPeekResult{};

		using MacroDefinitions = std::unordered_map<Tokenizer::Identifier::IdentString, Macro>;
		MacroDefinitions m_MacroDefinitions{};

		static constexpr char VA_ARGS_MACRO_NAME[12]{"__VA_ARGS__"};

		[[nodiscard]]
		Tokenizer::Token GetNextToken();

		[[nodiscard]]
		static inline bool CheckArgumentCountValidity(bool isVA, size_t paramCount, size_t argCount) noexcept;

		[[nodiscard]]
		bool
		GatherArgumentList(bool isVA, size_t numParameters, Preprocessor::MacroReplacementStackData &replacementData);

		[[nodiscard]]
		bool ExecuteMacroDefinition();

		[[nodiscard]]
		bool ExecuteIncludeDirective(Tokenizer::Token const &includeDirectiveToken);

		// In the case of nested macros, arguments need to be expanded before the nested macros are called because
		// the parameter names are not valid in the scope of the nested macro.
		//
		// Example:
		// #define OP(a, b, c) MULT(a, ADD(b, c))
		// c is not a parameter of ADD
		void ExpandMacroArguments(MacroReplacementStackData::ArgumentList &argList);

		[[nodiscard]]
		bool StartMacroExpansion(Macro const &macro);

		[[nodiscard]]
		bool MacroRecursionCheck(Span const &currentSpan, Tokenizer::Identifier::IdentString const &identifier);

		[[nodiscard]]
		bool HandleMacroParameterUsage(Tokenizer::Identifier::IdentString const &parameterName);

		[[nodiscard]]
		static std::optional<int> FindParameterIndexByName(
		        Preprocessor::FunctionLikeMacro::ParameterList const &parameterList,
		        Tokenizer::Identifier::IdentString const             &identifier
		) noexcept;

		[[nodiscard]]
		Tokenizer::Token ExecuteHashOperator();

	public:
		Preprocessor(std::string &&fileName, std::istream &iStream, Diagnosis::Vec &diagnoses)
		    : m_MainTokenizer{fileName, iStream, diagnoses}
		    , m_CurrentTokenizer{&m_MainTokenizer}
		    , m_Diagnoses{diagnoses} {
		}

		              Preprocessor(Preprocessor const &) = delete;
		              Preprocessor(Preprocessor &&)      = delete;
		Preprocessor &operator=(Preprocessor const &)    = delete;
		Preprocessor &operator=(Preprocessor &&)         = delete;

		Tokenizer::Token operator()();
	};
}// namespace jcc

#endif//JCC_PREPROCESSOR_H
