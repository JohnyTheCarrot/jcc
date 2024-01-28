#ifndef JCC_REPORTING_H
#define JCC_REPORTING_H

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
		TK_Unrecognized,
		TK_UnknownEscapeSequence,
		TK_InvalidBaseDigit,
		TK_UnexpectedIntSuffixChar,
	};

	enum class Class {
		Warning,
		Error,
	};

	Span span_{};
	Class class_{};
	Kind kind_{};
	Data data0_{}, data1_{};

	[[nodiscard]]
	std::string ToString() const;
};

std::ostream &operator<<(std::ostream &os, const Diagnosis &diagnosis);

void Warn(const std::string &filePath, std::istream &inputStream, const SpanOld &span, const std::string &message);

[[noreturn]]
void Error(const std::string &filePath, std::istream &inputStream, const SpanOld &span, const std::string &message);

#endif//JCC_REPORTING_H
