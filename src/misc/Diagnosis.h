#ifndef JCC_DIAGNOSIS_H
#define JCC_DIAGNOSIS_H

#include "Span.h"
#include "config.h"
#include <fmt/color.h>
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

	Span  m_Span;
	Class m_Class{};
	Kind  m_Kind{};
	Data  m_Data0{}, m_Data1{};

	[[nodiscard]]
	String ToString() const;
};

namespace DiagnosticsUtils {
	constexpr fmt::color COLOR_NEUTRAL{fmt::color::dim_gray};

	constexpr fmt::color GetClassColor(Diagnosis::Class diagClass) noexcept {
		switch (diagClass) {
			case Diagnosis::Class::Warning:
				return fmt::color::orange;
			case Diagnosis::Class::Error:
			default:
				return fmt::color::red;
		}
	}

	void OutputLine(OStream &os, int lineNum, const std::string &line);

	void OutputHighlight(
	        OStream &os, const std::optional<int> &startChar, const std::optional<int> &endChar, int lineLength,
	        Diagnosis::Class diagClass
	);
}// namespace DiagnosticsUtils

OStream &operator<<(OStream &os, const Diagnosis &diagnosis);

std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind);

#endif//JCC_DIAGNOSIS_H
