//
// Created by tuurm on 1/31/2024.
//

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

std::optional<char> CharStream::Next() {
	if (!m_IStream.get(m_Current).good())
		return std::nullopt;

	return m_Current;
}

std::optional<char> CharStream::operator++() {
	return Next();
}

char CharStream::operator++(int) {
	const char c{m_Current};
	Next();
	return c;
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
