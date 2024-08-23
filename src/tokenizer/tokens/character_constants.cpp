#include "character_constants.h"
#include "escape_sequences.h"
#include "misc/Diagnosis.h"

#include <numeric>

namespace jcc::tokenizer::character_constants {
	constexpr int implMaxCharsInCharLiteral{sizeof(compiler_data_types::Int::type)};

	[[nodiscard]]
	std::optional<compiler_data_types::Char32::type>
	ReadSingleCharacter(CharIter &charIter, ConstantPrefix prefix, SpanMarker const &startMarker) {
		if (charIter == CharIter::end() || charIter->m_Char == '\n') {
			Span span{
			        charIter.GetFileName(), startMarker, charIter.GetSentinel().m_LastSpanMarker,
			        charIter.GetInput()->tellg(), charIter.GetInput()
			};

			throw FatalCompilerError{Diagnosis::Kind::TK_CharUnterminated, std::move(span)};
		}

		auto const c{charIter->m_Char};
		++charIter;

		if (c == '\\') {
			auto const backslashMarker{charIter.GetCurrentSpanMarker()};

			return escape_sequences::Tokenize(charIter, backslashMarker, prefix);
		}

		if (c == '\'') {
			return std::nullopt;
		}

		return static_cast<compiler_data_types::Char32::type>(c);
	}

	Token Tokenize(CharIter &charIter, ConstantPrefix prefix, SpanMarker const &startMarker) {
		if (charIter == CharIter::end()) {
			Span span{
			        charIter.GetFileName(), startMarker, charIter.GetCurrentSpanMarker(), charIter.GetInput()->tellg(),
			        charIter.GetInput()
			};

			throw FatalCompilerError{Diagnosis::Kind::UnexpectedEOF, std::move(span)};
		}

		std::vector<compiler_data_types::Char32::type> characters{};
		while (true) {
			auto const chOptional{ReadSingleCharacter(charIter, prefix, startMarker)};
			if (!chOptional.has_value())
				break;

			characters.push_back(chOptional.value());
		}

		Span span{
		        charIter.GetFileName(), startMarker, charIter.GetCurrentSpanMarker(), charIter.GetInput()->tellg(),
		        charIter.GetInput()
		};

		if (characters.empty()) {
			throw FatalCompilerError{Diagnosis::Kind::TK_CharNoValue, std::move(span)};
		}

		// A UTF-8, UTF-16, or UTF-32 character constant shall not contain more than one character.
		if (characters.size() > 1 && prefix != ConstantPrefix::None) {
			throw FatalCompilerError{Diagnosis::Kind::TK_UTFCharMoreThanOneChar, std::move(span)};
		}

		// The value of a multi-character integer character constant is implementation-defined.
		// This compiler allows as many characters as there are bytes in the integer type.
		if (characters.size() > implMaxCharsInCharLiteral) {
			throw FatalCompilerError{Diagnosis::Kind::TK_IntegerCharMoreThanMaxChars, std::move(span)};
		}

		auto const result{std::accumulate(
		        characters.begin(), characters.end(), compiler_data_types::Char32::type{0},
		        [](auto acc, auto ch) { return (acc << (sizeof(char) * 8)) | ch; }
		)};

		return Token{
		        .m_Value = CharacterConstant{.m_Character = result, .m_Prefix = prefix}, .m_Span = std::move(span)
		};
	}
}// namespace jcc::tokenizer::character_constants
