//
// Created by tuurm on 04/07/2023.
//

#include "reporting.h"
#include "Span.h"
#include <iostream>

void Path(const std::string &filePath, const Span &span) {
    std::cout << filePath << "(" << span.GetLineNumber() << ':' << span.GetColumn() << "): ";
}

void Warn(const std::string &filePath, std::istream &inputStream, const Span &span, const std::string &message) {
    Path(filePath, span);
    std::cout << "Warning: "
              << message << std::endl
              << span.GetTextVersion();
}

[[noreturn]]
void Error(const std::string &filePath, std::istream &inputStream, const Span &span, const std::string &message) {
    Path(filePath, span);
    std::cout << "Error: "
              << message << std::endl
              << span.GetTextVersion();

    exit(1);
}