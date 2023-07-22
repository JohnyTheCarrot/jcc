//
// Created by tuurm on 04/07/2023.
//

#ifndef JCC_SPAN_H
#define JCC_SPAN_H


#include <string>

class Span final {
public:
    Span() : lineNumber{}, lineStartIndex{}, startCharacterIndex{}, length{1} {};

    Span(size_t line, size_t lineStartIndex, size_t characterIndex, size_t length, std::istream &inputStream);

    [[nodiscard]]
    const std::string &GetTextVersion() const { return this->textVersion; };

    [[nodiscard]]
    size_t GetLineNumber() const { return this->lineNumber; }

    [[nodiscard]]
    size_t GetLineStartIndex() const { return this->lineStartIndex; }

    [[nodiscard]]
    size_t GetStartCharacterIndex() const { return this->startCharacterIndex; }

    [[nodiscard]]
    size_t GetColumn() const { return this->startCharacterIndex - this->lineStartIndex; }

    [[nodiscard]]
    size_t GetLength() const { return this->length; }

    [[nodiscard]]
    size_t GetEndCharacterIndex() const { return this->startCharacterIndex + this->length; }

private:
    [[nodiscard]]
    std::string ToString(std::istream &inputStream) const;

    size_t lineNumber;
    size_t lineStartIndex;
    size_t startCharacterIndex;
    size_t length;
    std::string textVersion;
};


#endif //JCC_SPAN_H
