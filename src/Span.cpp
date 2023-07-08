#include "Span.h"
#include <istream>
#include <format>

Span::Span(size_t line, size_t lineStartIndex, size_t characterIndex, size_t length)
        : lineNumber{line}, lineStartIndex{lineStartIndex}, startCharacterIndex{characterIndex}, length{length} {

}

std::string Span::ToString(std::istream &inputStream) const {
    std::string buffer{};

    std::streamoff inputStreamPos{inputStream.tellg()};
    inputStream.seekg(this->lineStartIndex, std::istream::beg);
    getline(inputStream, buffer);
    inputStream.seekg(inputStreamPos, std::istream::beg);

    size_t column{this->startCharacterIndex - this->lineStartIndex};

    return std::format("{}\n{:>{}}{:->{}}\n{:>{}} ({}:{})\n", std::string{buffer}, '-', column - 1, ' ',
                       this->length, '^', (column - 1) + this->length / 2,
                       this->lineNumber, column);
}
