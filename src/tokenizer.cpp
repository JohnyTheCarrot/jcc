//
// Created by tuurm on 03/07/2023.
//

#include <istream>
#include <fmt/format.h>
#include "tokenizer.h"
#include "../libs/magic_enum/magic_enum.hpp"
#include "reporting.h"
#include "Span.h"
#include <cassert>
#include <memory>

size_t Tokenizer::TokenizeNormalToken() {
    std::streamoff inputStreamPos{this->inputStream.tellg()};
    char tokenBuffer[MAX_TOKEN_LENGTH + 1]{this->currentChar};
    this->inputStream.readsome(tokenBuffer + 1, MAX_TOKEN_LENGTH);
    tokenBuffer[MAX_TOKEN_LENGTH] = '\0';
    this->inputStream.seekg(inputStreamPos - 1, std::istream::beg);
    this->currentToken = std::nullopt;

    std::optional<long> optionalFoundTokenLength{std::nullopt};
    for (size_t index{0}; index < MAX_TOKEN_LENGTH; ++index) {
        if (tokenBuffer[index] == '\0')
            break;

        frozen::string key{tokenBuffer, index + 1};

        if (TOKEN_DEFINITIONS.contains(key)) {
            TokenType token{TOKEN_DEFINITIONS.at(key)};
            this->currentToken = token;
            optionalFoundTokenLength = index + 1;
        }
    }

    this->inputStream.seekg(optionalFoundTokenLength.value_or(1), std::istream::cur);

    return optionalFoundTokenLength.value_or(0);
}

void Tokenizer::TokenizeIdentifier(TokenList &tokensOut) {
    std::string identifierBuffer{this->currentChar};
    size_t currentLine{this->line};
    size_t currentLineStartIndex{this->lineStartIndex};
    size_t currentCharIndex{GetCharIndex()};

    while (GetNextChar()) {
        if (isalnum(this->currentChar) || this->currentChar == '_') {
            identifierBuffer += this->currentChar;

            continue;
        }

        this->inputStream.putback(this->currentChar);
        break;
    }

    if (identifierBuffer.length() > 0) {
        TokenPtr token{std::make_unique<Token>(TokenType::Identifier,
                                               Span(currentLine, currentLineStartIndex, currentCharIndex,
                                                    identifierBuffer.length(), this->inputStream), identifierBuffer)};
        tokensOut.push_back(std::move(token));

        return;
    }

    this->inputStream.putback(this->currentChar);
}

void Tokenizer::TokenizeString(TokenList &tokensOut) {
    std::string stringLiteralBuffer;
    bool isEscaped{false};
    bool wasTerminated{false};

    while (GetNextChar()) {
        if (this->currentChar == '\\') {
            isEscaped = true;
            continue;
        }

        if (isEscaped) {
            switch (this->currentChar) {
                case '\n':
                    continue;
                case 'n':
                    stringLiteralBuffer += '\n';
                    break;
                case 't':
                    stringLiteralBuffer += '\t';
                    break;
                case 'r':
                    stringLiteralBuffer += '\r';
                    break;
                case '0':
                    stringLiteralBuffer += '\0';
                    break;
                case '\\':
                    stringLiteralBuffer += '\\';
                    break;
                case '"':
                    stringLiteralBuffer += '"';
                    break;
                default:
                    Warn(this->filePath, this->inputStream,
                         Span(this->line, this->lineStartIndex, GetCharIndex() - 1, 2, this->inputStream),
                         fmt::format("Unrecognized escape sequence: '\\{}'", this->currentChar));
                    stringLiteralBuffer += this->currentChar;
                    break;
            }
            isEscaped = false;
            continue;
        }

        if (this->currentChar == '\n') {
            Error(this->filePath, this->inputStream,
                  Span(this->line, this->lineStartIndex, GetCharIndex(), 1, this->inputStream),
                  "Unexpected newline.");
        }

        if (this->currentChar == '"') {
            wasTerminated = true;
            break;
        }

        stringLiteralBuffer += this->currentChar;
    }

    if (!wasTerminated) {
        Error(this->filePath, this->inputStream,
              Span(this->line, this->lineStartIndex, GetCharIndex(), 1, this->inputStream),
              "Unexpected end of input.");
    }

    TokenPtr token{std::make_unique<Token>(TokenType::StringLiteral,
                                           SpanFromCurrent(stringLiteralBuffer.length()), stringLiteralBuffer)};
    tokensOut.push_back(std::move(token));
}

BaseConvertResult ConvertToBase(int base, char digit, int &digitOut) {
    assert(base <= 16);
    constexpr char NUMBERS[] = "0123456789abcdef";

    digit = static_cast<char>(tolower(digit));

    if (!isxdigit(digit)) {
        return BaseConvertResult::InvalidDigit;
    }

    if (digit > NUMBERS[base - 1]) {
        return BaseConvertResult::DigitNotInBase;
    }

    if (isdigit(digit)) {
        digitOut = digit - '0';
    } else {
        digitOut = 10 + (digit - 'a');
    }

    return BaseConvertResult::Success;
}

BaseConvertResult Tokenizer::GetDigit(int base, int &digitOut) {
    char digit{};
    bool peekResult{PeekNextChar(digit)};
    if (!peekResult) {
        return BaseConvertResult::UnexpectedEndOfInput;
    }

    BaseConvertResult convertResult;
    if ((convertResult = ConvertToBase(base, digit, digitOut)) != BaseConvertResult::Success) {
        return convertResult;
    }

    this->currentChar = digit;
    GetNextChar();
    return BaseConvertResult::Success;
}

IntLiteralSuffix
Tokenizer::GetIntLiteralSuffix(IntLiteralSuffix previousSuffix, bool &isSigned_Out, IntegerLiteralType &typeOut) {
    std::string errorString{"Invalid integer literal suffix."};

    if (ConsumeIfNextChar('u')) {
        if (previousSuffix == IntLiteralSuffix::Unsigned)
            Error(this->filePath, this->inputStream,
                  SpanFromCurrent(1),
                  errorString);

        isSigned_Out = true;
        return IntLiteralSuffix::Unsigned;
    }

    if (previousSuffix == IntLiteralSuffix::Long || previousSuffix == IntLiteralSuffix::LongLong)
        Error(this->filePath, this->inputStream,
              SpanFromCurrent(1),
              errorString);

    if (ConsumeIfNextChar('l')) {
        if (ConsumeIfNextChar('l')) {
            typeOut = IntegerLiteralType::LongLong;
            return IntLiteralSuffix::LongLong;
        }

        typeOut = IntegerLiteralType::Long;
        return IntLiteralSuffix::Long;
    }

    char nextChar{};
    if (PeekNextChar(nextChar) && isalpha(nextChar)) {
        GetNextChar();
        Error(this->filePath, this->inputStream,
              SpanFromCurrent(1),
              errorString);
    }

    return IntLiteralSuffix::None;
}

// todo: handle literal > type_max
void Tokenizer::TokenizeInteger(TokenList &tokensOut) {
    int base{10};

    if (this->currentChar == '0') {
        if (ConsumeIfNextChar('x')) {
            base = 16;
        } else {
            base = 8;
        }
    }

    std::vector<int> integerLiteralDigits{};
    if (base == 10) {
        int firstDigit{};
        ConvertToBase(base, this->currentChar, firstDigit);
        integerLiteralDigits.push_back(firstDigit);
    }

    int digit{};
    BaseConvertResult convertResult;
    while ((convertResult = GetDigit(base, digit)) == BaseConvertResult::Success) {
        integerLiteralDigits.push_back(digit);
    }

    bool isUnsigned{false};
    IntegerLiteralType type{};

    IntLiteralSuffix suffixOne{GetIntLiteralSuffix(IntLiteralSuffix::None, isUnsigned, type)};
    if (suffixOne != IntLiteralSuffix::None) {
        GetIntLiteralSuffix(suffixOne, isUnsigned, type);
    }

    size_t amountOfDigits{integerLiteralDigits.size()};
    if (convertResult == BaseConvertResult::DigitNotInBase) {
        Error(this->filePath, this->inputStream,
              Span(this->line, this->lineStartIndex, GetCharIndex() + 1, 1, this->inputStream),
              fmt::format("Invalid digit for base {}.", base));
    }

    IntegerTokenValue integerLiteralValue{0};
    for (size_t index{0}; index < amountOfDigits; ++index) {
        integerLiteralValue +=
                integerLiteralDigits[index] * static_cast<IntegerTokenValue>(pow(base, amountOfDigits - index - 1));
    }

    IntegerLiteralTokenValue value{integerLiteralValue, isUnsigned, type};
    TokenPtr token{std::make_unique<Token>(TokenType::IntegerLiteral,
                                           SpanFromCurrent(amountOfDigits), value)};
    tokensOut.push_back(std::move(token));
}

bool Tokenizer::TokenizeConstant(TokenList &tokensOut) {
    if (isdigit(this->currentChar)) {
        TokenizeInteger(tokensOut);

        return true;
    }

    if (this->currentChar == '"') {
        TokenizeString(tokensOut);

        return true;
    }

    return false;
}

void Tokenizer::Tokenize(TokenList &tokensOut) {
    while (GetNextChar()) {
        if (isspace(this->currentChar)) {
            continue;
        }

        // Comments
        if (this->currentChar == '/' && ConsumeIfNextChar('/')) {
            this->inputStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            ++this->line;
            this->lineStartIndex = GetCharIndex();
            this->currentToken = std::nullopt;
            continue;
        }

        // Normal tokens
        size_t foundTokenLength{TokenizeNormalToken()};

        if (this->currentToken.has_value()) {
            TokenPtr token{std::make_unique<Token>(this->currentToken.value(),
                                                   SpanFromCurrent(foundTokenLength))};
            tokensOut.push_back(std::move(token));

            continue;
        }

        // Identifiers
        if (isalpha(this->currentChar) || this->currentChar == '_') {
            TokenizeIdentifier(tokensOut);

            continue;
        }

        // String literals
        bool didMatch{TokenizeConstant(tokensOut)};
        if (didMatch) {
            continue;
        }

        std::string errorMessage{fmt::format("Unrecognized value: '{}'", this->currentChar)};
        Error(filePath, inputStream, SpanFromCurrent(), errorMessage);
    }
}

bool Tokenizer::GetNextChar() {
    bool readResult{this->inputStream.get(this->currentChar)};

    if (!readResult) {
        return false;
    }

    if (this->currentChar == '\n') {
        ++this->line;
        this->lineStartIndex = GetCharIndex();
    }

    return true;
}

bool Tokenizer::PeekNextChar(char &out) {
    char nextChar{static_cast<char>(this->inputStream.peek())};
    if (!this->inputStream) {
        return false;
    }

    out = nextChar;
    return true;
}

bool Tokenizer::ConsumeIfNextChar(char c, bool toLower) {
    char nextChar{};
    if (!PeekNextChar(nextChar)) {
        return false;
    }

    if (toLower) {
        nextChar = static_cast<char>(tolower(nextChar));
    }

    if (nextChar == c) {
        GetNextChar();
        return true;
    }

    return false;
}

size_t Tokenizer::GetCharIndex() const {
    int charIndex{static_cast<int>(this->inputStream.tellg()) - 1};

    return std::max(0, charIndex);
}

size_t Tokenizer::GetColumnIndex() const {
    return GetCharIndex() - this->lineStartIndex;
}

Span Tokenizer::SpanFromCurrent(size_t length) const {
    return {this->line, this->lineStartIndex, GetCharIndex(), length, this->inputStream};
}
