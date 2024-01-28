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
	String buffer{};
	std::vector<Preprocessor::Token> tokens{};
	Punctuator lastPunctuator{Punctuator::None};

	for (String::const_iterator current{m_Buffer.cbegin()}; current != m_Buffer.cend(); ++current) {
		if (*current != C('\n') && isspace(*current)) {
			continue;
		}

		if (lastPunctuator != Punctuator::None) {
			Punctuator punctuator{HandlePreviousPunctuator(current, lastPunctuator)};

			if (punctuator < Punctuator::None) {
				tokens.emplace_back(punctuator);
				lastPunctuator = Punctuator::None;
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
				break;
			case C('^'):
				lastPunctuator = Punctuator::Caret;
				break;
			case C('/'):
				lastPunctuator = Punctuator::Slash;
				break;
			case C(','):
				lastPunctuator = Punctuator::Comma;
				break;
			case C(':'):
				lastPunctuator = Punctuator::Colon;
				break;
			case C(';'):
				lastPunctuator = Punctuator::Semicolon;
				break;
			case C('#'):
				lastPunctuator = Punctuator::Hash;
				break;
			default:
				lastPunctuator = Punctuator::None;
				continue;
		}
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

std::ostream &operator<<(std::ostream &os, Preprocessor::Punctuator punctuator) {
	return os << magic_enum::enum_name(punctuator);
}
