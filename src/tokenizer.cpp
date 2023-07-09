//
// Created by tuurm on 03/07/2023.
//

#include <istream>
#include <format>
#include "tokenizer.h"
#include "../libs/magic_enum/magic_enum.hpp"
#include "reporting.h"
#include "Span.h"

size_t Tokenizer::TokenizeNormalToken() {
    std::streamoff inputStreamPos{this->inputStream.tellg()};
    char tokenBuffer[MAX_TOKEN_LENGTH + 1]{this->currentChar};
    this->inputStream.readsome(tokenBuffer + 1, MAX_TOKEN_LENGTH);
    tokenBuffer[MAX_TOKEN_LENGTH] = '\0';
    this->inputStream.seekg(inputStreamPos - 1);
    this->currentToken = std::nullopt;

    std::optional<size_t> optionalFoundTokenLength{std::nullopt};
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

void Tokenizer::TokenizeIdentifier() {
    std::string identifierBuffer{this->currentChar};

    while (this->GetNextChar()) {
        if (isalnum(this->currentChar) || this->currentChar == '_') {
            identifierBuffer += this->currentChar;

            continue;
        }

        this->inputStream.putback(this->currentChar);
        break;
    }

    if (identifierBuffer.length() > 0) {
        TokenPtr token{std::make_unique<Token>(TokenType::Identifier,
                                               this->SpanFromCurrent(identifierBuffer.length()), identifierBuffer)};
        this->tokensOut.push_back(std::move(token));

        return;
    }

    this->inputStream.putback(this->currentChar);
}

void Tokenizer::TokenizeString() {
    std::string stringLiteralBuffer;
    bool isEscaped{false};
    bool wasTerminated{false};

    while (this->GetNextChar()) {
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
                         Span(this->line, this->lineStartIndex, this->GetCharIndex() - 1, 2),
                         std::format("Unrecognized escape sequence: '\\{}'", this->currentChar));
                    stringLiteralBuffer += this->currentChar;
                    break;
            }
            isEscaped = false;
            continue;
        }

        if (this->currentChar == '\n') {
            Error(this->filePath, this->inputStream,
                  Span(this->line, this->lineStartIndex, this->GetCharIndex(), 1),
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
              Span(this->line, this->lineStartIndex, this->GetCharIndex(), 1),
              "Unexpected end of input.");
    }

    TokenPtr token{std::make_unique<Token>(TokenType::StringLiteral,
                                           this->SpanFromCurrent(stringLiteralBuffer.length()), stringLiteralBuffer)};
    this->tokensOut.push_back(std::move(token));
}

void Tokenize(const std::string &filePath, std::istream &inputStream, TokenList &tokensOut) {
    TokenizerContext context{
            .filePath = filePath,
            .inputStream = inputStream,
            .currentChar = '\0',
            .line = 1,
            .column = 0,
            .lineStartIndex = 0,
            .charIndex = 0,
            .currentToken = std::nullopt,
            .tokensOut = tokensOut
    };

bool Tokenizer::TokenizeConstant() {
    if (this->currentChar == '"') {
        TokenizeString();

        return true;
    }

    return false;
}

void Tokenizer::Tokenize() {
    while (this->GetNextChar()) {
        if (isspace(this->currentChar)) {
            continue;
        }

        // Comments
        char nextChar{};
        if (this->PeekNextChar(nextChar) && this->currentChar == '/' && nextChar == '/') {
            this->inputStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            ++this->line;
            this->lineStartIndex = this->GetCharIndex();
            this->currentToken = std::nullopt;
            continue;
        }

        // Normal tokens
        size_t foundTokenLength{TokenizeNormalToken()};

        if (this->currentToken.has_value()) {
            TokenPtr token{std::make_unique<Token>(this->currentToken.value(),
                                                   this->SpanFromCurrent(foundTokenLength))};
            this->tokensOut.push_back(std::move(token));

            continue;
        }

        // Identifiers
        if (isalpha(this->currentChar) || this->currentChar == '_') {
            TokenizeIdentifier();

            continue;
        }

        // String literals
        bool didMatch{TokenizeConstant()};
        if (didMatch) {
            continue;
        }

        std::string errorMessage{std::format("Unrecognized token: '{}'", this->currentChar)};
        Error(filePath, inputStream, this->SpanFromCurrent(), errorMessage);
    }

    tokensOut = std::move(this->tokensOut);
}

bool Tokenizer::GetNextChar() {
    bool readResult{this->inputStream.get(this->currentChar)};

    if (!readResult) {
        return false;
    }

    if (this->currentChar == '\n') {
        ++this->line;
        this->lineStartIndex = this->GetCharIndex();
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

size_t Tokenizer::GetCharIndex() const {
    int charIndex{static_cast<int>(this->inputStream.tellg()) - 1};

    return std::max(0, charIndex);
}

size_t Tokenizer::GetColumnIndex() const {
    return this->GetCharIndex() - this->lineStartIndex;
}

Span Tokenizer::SpanFromCurrent(size_t length) const {
    return {this->line, this->lineStartIndex, this->GetCharIndex(), length};
}
