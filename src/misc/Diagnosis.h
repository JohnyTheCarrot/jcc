#ifndef JCC_DIAGNOSIS_H
#define JCC_DIAGNOSIS_H

#include "Span.h"
#include "config.h"
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Diagnosis final {
	using Vec  = std::vector<Diagnosis>;
	using Data = std::optional<std::variant<char, std::string>>;

	enum class Kind {
		TK_StrUnterminated,
		TK_CharUnterminated,
		TK_CharNoValue,
		TK_CharOutOfRange,
		TK_CharHexNoDigits,
		TK_UnknownEscapeSequence,
		TK_UnexpectedEOF,
		TK_PartialTokenEncountered,
		TK_UnknownDirective,
		TK_EscapeExpectedNewline,
		TK_Unrecognized,
		TK_InvalidBaseDigit,
		TK_UnexpectedIntSuffixChar,
		TK_InvalidUniversalCharacterName,
		TK_IllegalUniversalCharacterName,
		TK_IllegalBackslash,
	};

	enum class Class {
		Warning,
		Error,
	};

	Span  m_Span{};
	Class m_Class{};
	Kind  m_Kind{};
	Data  m_Data0{}, m_Data1{};

	[[nodiscard]]
	String ToString() const;
};

OStream &operator<<(OStream &os, const Diagnosis &diagnosis);

std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind);

#endif//JCC_DIAGNOSIS_H
