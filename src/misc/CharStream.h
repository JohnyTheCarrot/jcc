//
// Created by tuurm on 1/31/2024.
//

#ifndef JCC_CHARSTREAM_H
#define JCC_CHARSTREAM_H

#include <optional>
#include <sstream>
#include <variant>

class CharStream final {
	std::istream &m_IStream;
	char          m_Current{};

public:
	explicit CharStream(std::istream &iStream)
	    : m_IStream{iStream}
	    , m_Current{'\0'} {
	}

	[[nodiscard]]
	bool Good() const noexcept;

	explicit operator bool() const noexcept;

	[[nodiscard]]
	bool
	operator!() const noexcept;

	[[nodiscard]]
	char
	operator*() const noexcept;

	[[nodiscard]]
	bool
	operator==(char c) const noexcept;

	[[nodiscard]]
	char Get() const noexcept;

	std::optional<char> Next();

	std::optional<char> operator++();

	char operator++(int);
};

std::string &operator+=(std::string &str, std::optional<char> c);

std::string &operator+=(std::string &str, const CharStream &charStream);

#endif//JCC_CHARSTREAM_H
