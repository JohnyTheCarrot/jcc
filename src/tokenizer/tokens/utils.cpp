#include "utils.h"
#include "escape_sequences.h"
#include "misc/Diagnosis.h"
#include "misc/Span.h"
#include "tokenizer/char_iter.h"

namespace jcc::tokenizer::utils {
	[[nodiscard]]
	std::optional<compiler_data_types::Char32::type> ReadSingleCharacter(
	        CharIter &charIter, ConstantPrefix prefix, SpanMarker const &startMarker, ConstantType constantType
	) {
		if (charIter == CharIter::end() || charIter->m_Char == '\n') {
			Span span{
			        charIter.GetFileName(), startMarker, charIter.GetSentinel().m_LastSpanMarker,
			        charIter.GetInput()->tellg(), charIter.GetInput()
			};

			auto const diagnosisKind{
			        constantType == ConstantType::Character ? Diagnosis::Kind::TK_CharUnterminated
			                                                : Diagnosis::Kind::TK_StrUnterminated
			};

			throw FatalCompilerError{diagnosisKind, std::move(span)};
		}

		// TODO: UTF-8, UTF-16 and UTF-32
		auto const c{charIter->m_Char};
		++charIter;

		if (c == '\\') {
			auto const backslashMarker{charIter.GetCurrentSpanMarker()};

			return escape_sequences::Tokenize(charIter, backslashMarker, prefix);
		}

		if (auto const terminator{constantType == ConstantType::Character ? '\'' : '"'}; c == terminator) {
			return std::nullopt;
		}

		return static_cast<compiler_data_types::Char32::type>(c);
	}

	bool IsNonDigit(char c) {
		return std::isalpha(c) || c == '_';
	}
}// namespace jcc::tokenizer::utils
