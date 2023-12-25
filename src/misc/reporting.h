//
// Created by tuurm on 04/07/2023.
//

#ifndef JCC_REPORTING_H
#define JCC_REPORTING_H

#include "Span.h"
#include <optional>
#include <stdnoreturn.h>
#include <string>
#include <variant>
#include <vector>

struct Diagnosis final {
	using Vec = std::vector<Diagnosis>;
	using Data = std::optional<std::variant<char, std::string>>;
	enum class Kind {
		TK_Unrecognized,
		TK_UnknownEscapeSequence,
		TK_StrUnterminated,
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

	Diagnosis(const Span &span, Class diagClass, Kind kind, const Data &data0, const Data &data1)
		: span_{span}
		, class_{diagClass}
		, kind_{kind}
		, data0_{data0}
		, data1_{data1} {}

	[[nodiscard]]
	std::string ToString() const;
};

void Warn(const std::string &filePath, std::istream &inputStream, const Span &span, const std::string &message);

[[noreturn]]
void Error(const std::string &filePath, std::istream &inputStream, const Span &span, const std::string &message);

#endif//JCC_REPORTING_H
