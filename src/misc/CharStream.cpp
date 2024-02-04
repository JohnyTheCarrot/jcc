#include "CharStream.h"

bool CharStream::Good() const noexcept {
	return m_IStream.good();
}

CharStream::operator bool() const noexcept {
	return Good();
}

bool CharStream::operator!() const noexcept {
	return !Good();
}

char CharStream::operator*() const noexcept {
	return Get();
}

bool CharStream::operator==(char c) const noexcept {
	return Good() && Get() == c;
}

char CharStream::Get() const noexcept {
	return m_Current;
}

std::optional<char> CharStream::SimpleNext() {
	m_IsEscapeChar = false;

	if (!m_IStream.get(m_Current).good())
		return std::nullopt;

	return m_Current;
}

CharStream::NextChar CharStream::Next() {
	if (!SimpleNext().has_value())
		return std::nullopt;

	if (m_Current != '\\')
		return m_Current;

	const auto next{SimpleNext()};
	if (!next.has_value())
		throw InvalidBackslashException();

	switch (*next) {
		case '\r':
			if (!SimpleNext().has_value() || m_Current != '\n')
				throw InvalidBackslashException();

			if (const auto afterCr{SimpleNext()}; afterCr.has_value()) {
				return afterCr.value();
			}

			throw InvalidBackslashException();
		case '\n':
			if (const auto afterNl{SimpleNext()}; afterNl.has_value()) {
				return afterNl.value();
			}

			throw InvalidBackslashException();
		case 'u':
		case 'U':
			m_IsEscapeChar = true;
			return next;
		default:
			throw InvalidBackslashException();
	}
}

CharStream::NextChar CharStream::operator++() {
	return Next();
}

char CharStream::operator++(int) {
	const char c{m_Current};
	Next();
	return c;
}

bool CharStream::GetIsEscapeChar() const noexcept {
	return m_IsEscapeChar;
}

std::string &operator+=(std::string &str, std::optional<char> c) {
	if (c.has_value())
		str += c.value();

	return str;
}

std::string &operator+=(std::string &str, const CharStream &charStream) {
	str += charStream.Get();
	return str;
}
