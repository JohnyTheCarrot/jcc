#include "CharStream.h"

namespace jcc {
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

		m_CurrentSpanMarker.NextChar();
	}

	void CharStream::NextLine() noexcept {
		m_CurrentSpanMarker.NextLine();
	}

	char CharStream::Next() {
		m_PreviousSpanMarker = m_CurrentSpanMarker;

		while (true) {
			if (m_WasLastCharNewLine) {
				NextLine();
				m_WasLastCharNewLine = false;
			}

			InternalNext();

			if (m_Current == '\r') {
				InternalNext();
				m_WasLastCharNewLine = true;
				return '\n';
			}

			if (m_Current == '\n') {
				m_WasLastCharNewLine = true;
				return '\n';
			}

			if (m_Current == '\t') {
				// See the following SO post for context. https://stackoverflow.com/a/13094734
				// We're going up to 7 because the tab is already the first character
				for (size_t idx{}; idx < 7; ++idx) m_CurrentSpanMarker.NextChar(false);
				return '\t';
			}

			if (m_Current != '\\') {
				return m_Current;
			}

			auto const next{m_IStream.peek()};
			if (next == '\r') {
				InternalNext();
				InternalNext();
				continue;
			}

			if (next == '\n') {
				InternalNext();
				NextLine();
				continue;
			}

			return '\\';
		}
	}

	CharStream::NextChar CharStream::operator++() {
		return Next();
	}

	char CharStream::operator++(int) {
		char const c{m_Current};
		Next();
		return c;
	}

	std::string &operator+=(std::string &str, std::optional<char> c) {
		if (c.has_value())
			str += c.value();

		return str;
	}

	std::string &operator+=(std::string &str, CharStream const &charStream) {
		str += charStream.Get();
		return str;
	}
}// namespace jcc
