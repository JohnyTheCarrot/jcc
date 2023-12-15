//
// Created by tuurm on 04/07/2023.
//

#ifndef JCC_SPAN_H
#define JCC_SPAN_H


#include <string>

class Span final {
public:
	Span()
		: _lineNumber{}
		, _lineStartIndex{}
		, _startCharacterIndex{}
		, _length{1}
		, _lineContent{"no content"}
		, _textVersion{"no content"} {};

	Span(size_t line, size_t lineStartIndex, size_t characterIndex, size_t length, std::istream &inputStream);

	[[nodiscard]]
	const std::string &GetTextVersion() const {
		return this->_textVersion;
	};

	Span operator+(const Span &other) const {
		Span result{};

		// todo: what if there is text in between?
		// todo: what if the spans are on different lines?
		result._startCharacterIndex = this->_startCharacterIndex;
		result._lineNumber = this->_lineNumber;
		result._length = this->_length + other._length;
		result._lineStartIndex = this->_lineStartIndex;
		result._lineContent = this->_lineContent;
		result._textVersion = result.ToString();

		return result;
	};

	const Span &operator+=(const Span &other) {
		if (!this->_isInitialized) {
			*this = other;
			return *this;
		}

		*this = *this + other;
		return *this;
	}

	[[nodiscard]]
	size_t GetLineNumber() const {
		return this->_lineNumber;
	}

	[[nodiscard]]
	size_t GetLineStartIndex() const {
		return this->_lineStartIndex;
	}

	[[nodiscard]]
	size_t GetStartCharacterIndex() const {
		return this->_startCharacterIndex;
	}

	[[nodiscard]]
	size_t GetColumn() const {
		return this->_startCharacterIndex - this->_lineStartIndex + 1;
	}

	[[nodiscard]]
	size_t GetLength() const {
		return this->_length;
	}

	[[nodiscard]]
	size_t GetEndCharacterIndex() const {
		return this->_startCharacterIndex + this->_length;
	}

private:
	[[nodiscard]]
	std::string ToString() const;

	bool _isInitialized{false};
	size_t _lineNumber;
	size_t _lineStartIndex;
	size_t _startCharacterIndex;
	size_t _length;
	std::string _lineContent{};
	std::string _textVersion{};
};


#endif//JCC_SPAN_H
