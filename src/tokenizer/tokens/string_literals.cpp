#include "string_literals.h"
#include "misc/Diagnosis.h"
#include "tokenizer/char_iter.h"
#include "utils.h"

namespace jcc::tokenizer::string_literals {
	Token Tokenize(CharIter &charIter, ConstantPrefix prefix, SpanMarker const &startMarker) {
		if (charIter == CharIter::end()) {
			Span span{
			        charIter.GetFileName(), startMarker, charIter.GetCurrentSpanMarker(), charIter.GetInput()->tellg(),
			        charIter.GetInput()
			};

			throw FatalCompilerError{Diagnosis::Kind::UnexpectedEOF, std::move(span)};
		}

		std::u32string u32Characters{};
		while (true) {
			auto const chOptional{utils::ReadSingleCharacter(charIter, prefix, startMarker, utils::ConstantType::String)
			};
			if (!chOptional.has_value())
				break;

			u32Characters.push_back(chOptional.value());
		}

		// TODO: this probably doesn't actually work
		std::string strContents{};
		std::ranges::copy(u32Characters, std::back_inserter(strContents));

		Span span{
		        charIter.GetFileName(), startMarker, charIter.GetCurrentSpanMarker(), charIter.GetInput()->tellg(),
		        charIter.GetInput()
		};

		return Token{.m_Value = StringConstant{std::move(strContents), prefix}, .m_Span = std::move(span)};
	}
}// namespace jcc::tokenizer::string_literals
