//
// Created by tuurm on 04/07/2023.
//

#ifndef JCC_REPORTING_H
#define JCC_REPORTING_H

#include <stdnoreturn.h>
#include <string>

class Span;

void Warn(const std::string &filePath, std::istream &inputStream, const Span &span, const std::string &message);

[[noreturn]]
void Error(const std::string &filePath, std::istream &inputStream, const Span &span, const std::string &message);

#endif//JCC_REPORTING_H
