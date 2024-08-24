#include "identifiers.h"
#include "misc/Diagnosis.h"

namespace jcc::tokenizer::identifiers {
	[[nodiscard]]
	std::optional<SpanMarker> CollectRestOfIdentifier(CharIter &charIter, std::string &identifier) {
		std::optional<SpanMarker> lastSpanMarker{};

		while (charIter != CharIter::c_UntilNewline) {
			auto const [spanMarker, character, isSentinel]{*charIter};

			if (!Identifier::IsValidChar(character))
				break;

			identifier.push_back(character);
			lastSpanMarker = spanMarker;
			++charIter;
		}

		return lastSpanMarker;
	}

	Token Tokenize(CharIter const &charIter, IdentifierTokenStart const &tokenStart) {
		if (charIter == CharIter::end()) {
			Span span{
			        charIter.GetFileName(), tokenStart.m_Start, charIter.GetSentinel().m_LastSpanMarker,
			        charIter.GetInput()->tellg(), charIter.GetInput()
			};

			// check if identifier contains a backslash
			if (tokenStart.m_Identifier.find('\\') != std::string::npos)
				// TODO: Universal character names
				throw FatalCompilerError{Diagnosis::Kind::TODO, std::move(span)};
			return Token{.m_Value = Identifier{tokenStart.m_Identifier}, .m_Span = std::move(span)};
		}

		std::string identifier{};
		auto const &[partialIdentifier, identifierStart, identifierStartPos]{tokenStart};

		Span span{
		        charIter.GetFileName(), identifierStart, identifierStart, charIter.GetInput()->tellg(),
		        charIter.GetInput()
		};

		identifier.reserve(partialIdentifier.length());
		std::ranges::move(partialIdentifier, std::back_inserter(identifier));

		// check if identifier contains a backslash
		if (identifier.find('\\') != std::string::npos)
			// TODO: Universal character names
			throw FatalCompilerError{Diagnosis::Kind::TODO, std::move(span)};

		Identifier identToken{std::move(identifier)};

		return Token{.m_Value = std::move(identToken), .m_Span = std::move(span)};
	}
}// namespace jcc::tokenizer::identifiers
