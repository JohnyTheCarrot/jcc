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

void CharStream::InternalNext() {
	if (!m_IStream.get(m_Current).good()) {
		m_Current = END_OF_FILE;
		return;
	}
}

char CharStream::Next() {
	while (true) {
		InternalNext();

		if (m_Current != '\\') {
			return m_Current;
		}

		const auto next{m_IStream.peek()};
		if (next == '\r') {
			InternalNext();
			InternalNext();
			continue;
		}

		if (next == '\n') {
			InternalNext();
			continue;
		}

		return '\\';
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
