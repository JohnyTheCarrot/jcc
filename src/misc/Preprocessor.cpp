#include "Preprocessor.h"
#include "reporting.h"
#include <magic_enum/magic_enum.hpp>
#include <regex>

std::tuple<Preprocessor::Punctuator, Preprocessor::Punctuator>
CollapsePartialPunctuator(Preprocessor::Punctuator punctuator) {
	using Punctuator = Preprocessor::Punctuator;

	switch (punctuator) {
		case Punctuator::PartialLeftShift:
			return {Punctuator::LessThanLessThan, Punctuator::None};
		case Punctuator::PartialRightShift:
			return {Punctuator::GreaterThanGreaterThan, Punctuator::None};
		case Punctuator::PartialHashHashDigraph:
			return {Punctuator::Hash, Punctuator::None};
		case Punctuator::HashHashDigraphFailure:
		case Punctuator::PartialHashHashDigraphStage2:
			return {Punctuator::Hash, Punctuator::Percent};
		case Punctuator::PartialEllipsis:
			return {Punctuator::Dot, Punctuator::Dot};
		default:
			return {punctuator, Punctuator::None};
	}
}

std::tuple<Preprocessor::Punctuator, Preprocessor::Punctuator> CollapseFailures(Preprocessor::Punctuator punctuator) {
	using Punctuator = Preprocessor::Punctuator;

	switch (punctuator) {
		case Punctuator::HashHashDigraphFailure:
			return {Punctuator::Hash, Punctuator::Percent};
		case Punctuator::EllipsisFailure:
			return {Punctuator::Dot, Punctuator::Dot};
		default:
			return {punctuator, Punctuator::None};
	}
}

Preprocessor::Punctuator HandlePreviousPunctuator(String::const_iterator &current, Preprocessor::Punctuator prev) {
	using Punctuator = Preprocessor::Punctuator;
	Punctuator result{};

	switch (prev) {
		case Punctuator::Minus:
			switch (*current) {
				case C('-'):
					result = Punctuator::MinusMinus;
					break;
				case C('>'):
					result = Punctuator::Arrow;
					break;
				case C('='):
					result = Punctuator::MinusEqual;
					break;
				default:
					return Punctuator::Minus;
			}
			break;
		case Punctuator::Plus:
			switch (*current) {
				case C('+'):
					result = Punctuator::PlusPlus;
					break;
				case C('='):
					result = Punctuator::PlusEqual;
					break;
				default:
					return Punctuator::Plus;
			}
			break;
		case Punctuator::Percent:
			switch (*current) {
				case C('>'):
					result = Punctuator::RightBrace;
					break;
				case C(':'):
					return Punctuator::PartialHashHashDigraph;
				case C('='):
					result = Punctuator::PercentEqual;
					break;
				default:
					return Punctuator::Percent;
			}
			break;
		case Punctuator::Equal:
			if (*current == C('='))
				result = Punctuator::EqualEqual;
			else
				return Punctuator::Equal;
			break;
		case Punctuator::Ampersand:
			switch (*current) {
				case C('&'):
					result = Punctuator::AmpersandAmpersand;
					break;
				case C('='):
					result = Punctuator::AmpersandEqual;
					break;
				default:
					return Punctuator::Ampersand;
			}
			break;
		case Punctuator::VerticalBar:
			switch (*current) {
				case C('|'):
					result = Punctuator::VerticalBarVerticalBar;
					break;
				case C('='):
					result = Punctuator::VerticalBarEqual;
					break;
				default:
					return Punctuator::VerticalBar;
			}
			break;
		case Punctuator::LessThan:
			switch (*current) {
				case C('<'):
					return Punctuator::PartialLeftShift;
				case C('='):
					result = Punctuator::LessThanEqual;
					break;
				case C(':'):
					result = Punctuator::LeftBracket;
					break;
				case C('%'):
					result = Punctuator::LeftBrace;
					break;
				default:
					return Punctuator::LessThan;
			}
			break;
		case Punctuator::GreaterThan:
			switch (*current) {
				case C('>'):
					return Punctuator::PartialRightShift;
				case C('='):
					result = Punctuator::GreaterThanEqual;
					break;
				case C(':'):
					result = Punctuator::RightBracket;
					break;
				default:
					return Punctuator::GreaterThan;
			}
			break;
		case Punctuator::PartialLeftShift:
			if (*current == C('='))
				result = Punctuator::LessThanLessThanEqual;
			else
				return Punctuator::LessThanLessThan;
			break;
		case Punctuator::PartialRightShift:
			if (*current == C('='))
				result = Punctuator::GreaterThanGreaterThanEqual;
			else
				return Punctuator::GreaterThanGreaterThan;
			break;
		case Punctuator::PartialHashHashDigraph:
			if (*current == C('%')) {
				return Punctuator::PartialHashHashDigraphStage2;
			}

			return Punctuator::Hash;
		case Punctuator::PartialHashHashDigraphStage2:
			if (*current == C(':')) {
				result = Punctuator::HashHash;
			} else {
				return Punctuator::HashHashDigraphFailure;
			}
			break;
		case Punctuator::Hash:
			if (*current == C('#')) {
				result = Punctuator::HashHash;
			} else {
				return Punctuator::Hash;
			}
			break;
		case Punctuator::ExclamationMark:
			if (*current == C('=')) {
				result = Punctuator::ExclamationMarkEqual;
			} else {
				return Punctuator::ExclamationMark;
			}
			break;
		case Punctuator::Dot:
			if (*current == C('.')) {
				return Punctuator::PartialEllipsis;
			}
			return Punctuator::Dot;
		case Punctuator::PartialEllipsis:
			if (*current == C('.')) {
				result = Punctuator::Ellipsis;
			} else {
				return Punctuator::EllipsisFailure;
			}
			break;
		case Punctuator::Asterisk:
			if (*current == C('=')) {
				result = Punctuator::AsteriskEqual;
			} else {
				return Punctuator::Asterisk;
			}
			break;
		case Punctuator::Slash:
			if (*current == C('=')) {
				result = Punctuator::SlashEqual;
			} else {
				return Punctuator::Slash;
			}
			break;
		case Punctuator::Caret:
			if (*current == C('=')) {
				result = Punctuator::CaretEqual;
			} else {
				return Punctuator::Caret;
			}
			break;
		case Punctuator::Colon:
			if (*current == C('>')) {
				result = Punctuator::RightBracket;
			} else {
				return Punctuator::Colon;
			}
			break;
		default:
			// the previousPunctuator is itself a complete punctuator
			return prev;
	}

	++current;
	return result;
}

Preprocessor::TokenList Preprocessor::Tokenize(Diagnosis::Vec &diagnoses) {
	TokenList tokens{};
	Punctuator lastPunctuator{Punctuator::None};
	bool wasLastPunctuatorHash{false};
	Token *lastHash{nullptr};

	for (String::const_iterator current{m_Buffer.cbegin()}; current != m_Buffer.cend(); ++current) {
		if (*current != C('\n') && isspace(*current)) {
			continue;
		}

		if (lastPunctuator != Punctuator::None) {
			Punctuator punctuator{HandlePreviousPunctuator(current, lastPunctuator)};

			if (punctuator < Punctuator::None) {
				Token &inserted{tokens.emplace_back(punctuator)};
				lastPunctuator = Punctuator::None;

				if (punctuator == Punctuator::Hash) {
					wasLastPunctuatorHash = true;
					lastHash              = &inserted;
				}
			}

			if (current == m_Buffer.cend()) {
				if (punctuator > Punctuator::None) {
					const auto [one, two] = CollapsePartialPunctuator(punctuator);
					if (one != Punctuator::None) {
						tokens.emplace_back(one);
					}
					lastPunctuator = two;
				} else
					lastPunctuator = Punctuator::None;
				break;
			}

			if (punctuator > Punctuator::None) {
				lastPunctuator = punctuator;
				continue;
			}
		}

		if (*current == C('\n')) {
			// TODO: Handle line continuations
			// TODO: invalid in string literals
			lastPunctuator        = Punctuator::None;
			wasLastPunctuatorHash = false;
			continue;
		}

		// Punctuators
		switch (*current) {
			case C('['):
				lastPunctuator = Punctuator::LeftBracket;
				continue;
			case C(']'):
				lastPunctuator = Punctuator::RightBracket;
				continue;
			case C('{'):
				lastPunctuator = Punctuator::LeftBrace;
				continue;
			case C('}'):
				lastPunctuator = Punctuator::RightBrace;
				continue;
			case C('('):
				lastPunctuator = Punctuator::LeftParenthesis;
				continue;
			case C(')'):
				lastPunctuator = Punctuator::RightParenthesis;
				continue;
			case C('.'):
				lastPunctuator = Punctuator::Dot;
				continue;
			case C('-'):
				lastPunctuator = Punctuator::Minus;
				continue;
			case C('+'):
				lastPunctuator = Punctuator::Plus;
				continue;
			case C('&'):
				lastPunctuator = Punctuator::Ampersand;
				continue;
			case C('|'):
				lastPunctuator = Punctuator::VerticalBar;
				continue;
			case C('*'):
				lastPunctuator = Punctuator::Asterisk;
				continue;
			case C('~'):
				lastPunctuator = Punctuator::Tilde;
				continue;
			case C('<'):
				lastPunctuator = Punctuator::LessThan;
				continue;
			case C('>'):
				lastPunctuator = Punctuator::GreaterThan;
				continue;
			case C('='):
				lastPunctuator = Punctuator::Equal;
				continue;
			case C('!'):
				lastPunctuator = Punctuator::ExclamationMark;
				continue;
			case C('%'):
				lastPunctuator = Punctuator::Percent;
				continue;
			case C('?'):
				lastPunctuator = Punctuator::QuestionMark;
				continue;
			case C('^'):
				lastPunctuator = Punctuator::Caret;
				continue;
			case C('/'):
				lastPunctuator = Punctuator::Slash;
				continue;
			case C(','):
				lastPunctuator = Punctuator::Comma;
				continue;
			case C(':'):
				lastPunctuator = Punctuator::Colon;
				continue;
			case C(';'):
				lastPunctuator = Punctuator::Semicolon;
				continue;
			case C('#'):
				lastPunctuator = Punctuator::Hash;
				continue;
			default:
				lastPunctuator = Punctuator::None;
				break;
		}

		// TODO: Handle literals, as the keyword/identifier code relies on the character being a valid initial character

		// Keywords or Identifiers
		const String::const_iterator end{std::find_if(current, m_Buffer.cend(), [](auto c) {
			return !isalnum(c) && c != C('_');
		})};
		if (end == current)
			continue;

		const StringView content(current, end);
		const String contentStr{content};

		if (wasLastPunctuatorHash) {
			const std::optional<Directive> directive{MatchDirective(content)};
			if (directive.has_value()) {
				*lastHash = directive.value();
				current   = end - 1;// -1 because the for loop will increment it
				continue;
			}
		}

		const std::optional<Keyword> keyword{MatchKeyword(content)};

		if (keyword.has_value()) {
			tokens.emplace_back(keyword.value());
		} else {
			tokens.emplace_back(Identifier{contentStr});
		}
		current = end - 1;// -1 because the for loop will increment it
	}

	const auto [firstPunctuator, secondPunctuator] = CollapsePartialPunctuator(lastPunctuator);

	if (firstPunctuator != Punctuator::None) {
		tokens.emplace_back(firstPunctuator);
	}

	if (secondPunctuator != Punctuator::None) {
		tokens.emplace_back(secondPunctuator);
	}

	return tokens;
}

Preprocessor::TokenList Preprocessor::Process(Diagnosis::Vec &diagnoses) {
	if (m_Buffer.empty())
		return TokenList{};

	m_Buffer = std::regex_replace(m_Buffer, Regex{C(R"(\r\n)")}, C("\n"));

	TokenList tokenList{Tokenize(diagnoses)};

	return tokenList;
}

std::optional<Preprocessor::Keyword> Preprocessor::MatchKeyword(const StringView &view) const {
	const auto matchIt{m_Keywords.find(view)};

	if (matchIt == m_Keywords.cend())
		return std::nullopt;

	return matchIt->second;
}

std::optional<Preprocessor::Directive> Preprocessor::MatchDirective(const StringView &view) const {
	const auto matchIt{m_Directives.find(view)};

	if (matchIt == m_Directives.cend())
		return std::nullopt;

	return matchIt->second;
}

std::ostream &operator<<(std::ostream &os, Preprocessor::Punctuator punctuator) {
	return os << magic_enum::enum_name(punctuator);
}

std::ostream &operator<<(std::ostream &os, Preprocessor::Keyword keyword) {
	return os << magic_enum::enum_name(keyword);
}

std::ostream &operator<<(std::ostream &os, Preprocessor::Directive directive) {
	return os << magic_enum::enum_name(directive);
}
