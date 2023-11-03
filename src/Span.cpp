#include "Span.h"
#include <istream>
#include "fmt/format.h"

Span::Span(size_t line, size_t lineStartIndex, size_t characterIndex, size_t length, std::istream &inputStream)
        : _lineNumber{line}, _lineStartIndex{lineStartIndex}, _startCharacterIndex{characterIndex}, _length{length} {
    std::streamoff inputStreamPos{inputStream.tellg()};
    inputStream.seekg(static_cast<long>(lineStartIndex), std::istream::beg);
    getline(inputStream, this->_lineContent, '\n');
    inputStream.seekg(inputStreamPos, std::istream::beg);

    this->_textVersion = this->ToString();
}

std::string Span::ToString() const {
    size_t column{ this->GetColumn() };

    return fmt::format("{}\n{:>{}}{:->{}}\n{:>{}} ({}:{})\n", std::string{this->_lineContent}, '-', column, ' ',
                       this->_length, '^', column + this->_length / 2,
                       this->_lineNumber, column);
}
