#ifndef JCC_REPORTING_H
#define JCC_REPORTING_H

#include "Config.h"
#include "Span.h"
#include "SpanOld.h"
#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Diagnosis final {
	using Vec  = std::vector<Diagnosis>;
	using Data = std::optional<std::variant<char, std::string>>;

	enum class Kind {
		PP_StrUnterminated,
		PP_CharUnterminated,
		PP_CharNoValue,
		PP_CharOutOfRange,
		PP_CharHexNoDigits,
		PP_UnknownEscapeSequence,
		PP_UnexpectedEOF,
		PP_PartialTokenEncountered,
		PP_UnknownDirective,
		PP_EscapeExpectedNewline,
		TK_Unrecognized,
		TK_InvalidBaseDigit,
		TK_UnexpectedIntSuffixChar,
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

void Warn(const std::string &filePath, std::istream &inputStream, const SpanOld &span, const std::string &message);

[[noreturn]]
void Error(const std::string &filePath, std::istream &inputStream, const SpanOld &span, const std::string &message);

#endif//JCC_REPORTING_H
