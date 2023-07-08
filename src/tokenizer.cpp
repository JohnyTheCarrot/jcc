//
// Created by tuurm on 03/07/2023.
//

#include <iostream>
#include <format>
#include "tokenizer.h"
#include "../libs/magic_enum/magic_enum.hpp"
#include "reporting.h"
#include "Span.h"

void Tokenize(const std::string &filePath, std::istream &inputStream, std::vector<Token> &tokensOut) {
    char currentChar;
    std::vector<Token> tokens;
    std::optional<TokenType> currentToken;
    size_t line{1};
    size_t lineStartIndex{0};
    size_t characterIndex{0};
    size_t column{0};

    while (inputStream.get(currentChar)) {
        if (currentChar == '\n') {
            ++line;
            ++characterIndex;
            lineStartIndex = characterIndex + 2;
            column = 0;
        }

        if (isspace(currentChar)) {
            ++column;
            ++characterIndex;
            continue;
        }

        std::streamoff inputStreamPos{inputStream.tellg()};
        char tokenBuffer[MAX_TOKEN_LENGTH + 1]{currentChar};
        inputStream.readsome(tokenBuffer + 1, MAX_TOKEN_LENGTH);
        tokenBuffer[MAX_TOKEN_LENGTH] = '\0';
        inputStream.seekg(inputStreamPos - 1);
        currentToken = std::nullopt;

        size_t foundTokenLength{1};
        for (size_t index{0}; index < MAX_TOKEN_LENGTH; ++index) {
            if (tokenBuffer[index] == '\0')
                break;

            frozen::string key{tokenBuffer, index + 1};

            if (TOKEN_DEFINITIONS.contains(key)) {
                TokenType token{TOKEN_DEFINITIONS.at(key)};
                currentToken = token;
                foundTokenLength = index + 1;
//                std::cout << "'" << std::string(key.data(), index + 1) << "' could be" << " : "
//                          << magic_enum::enum_name(token) << std::endl;
            }
        }

        inputStream.seekg(foundTokenLength, std::istream::cur);
        column += foundTokenLength;
        characterIndex += foundTokenLength;

        if (currentToken.has_value()) {
//            std::cout << "TokenType was " << magic_enum::enum_name(currentToken.value()) << std::endl;

            Token token{.type = currentToken.value(), .line = line, .column = column, .length = foundTokenLength};
            tokens.push_back(token);

            continue;
        }
        // todo: clean up: move to separate functions
        // Identifiers
        if (isalpha(currentChar) || currentChar == '_') {
            std::string identifierBuffer{currentChar};

            while (inputStream.get(currentChar)) {
                if (isalnum(currentChar) || currentChar == '_') {
                    identifierBuffer += currentChar;
                    ++column;
                    ++characterIndex;
                } else {
                    inputStream.putback(currentChar);
                    break;
                }
            }

            if (identifierBuffer.length() > 0) {
                Token token{.type = TokenType::Identifier, .line = line, .column = column, .length = identifierBuffer.length()};
                token.value = identifierBuffer;
                tokens.push_back(token);
                ++column;
                ++characterIndex;
//                std::cout << "Was identifier: " << identifierBuffer << std::endl;
            }

            continue;
        }

        if (currentChar == '"') {
            std::string stringLiteralBuffer;
            bool isEscaped{false};
            bool wasTerminated{false};

            while (inputStream.get(currentChar)) {
                ++column;
                ++characterIndex;
                if (currentChar == '\\') {
                    isEscaped = true;
                    continue;
                }

                if (isEscaped) {
                    switch (currentChar) {
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
                            Warn(filePath, inputStream, Span(line, lineStartIndex, characterIndex - 2, 2),
                                 std::format("Unrecognized escape sequence: '\\{}'", currentChar));
                            stringLiteralBuffer += currentChar;
                            break;
                    }
                    isEscaped = false;
                    continue;
                }

                if (currentChar == '\n') {
                    Error(filePath, inputStream, Span(line, lineStartIndex, characterIndex, 1),
                          "Unexpected newline.");
                }

                if (currentChar == '"') {
                    wasTerminated = true;
                    break;
                }

                stringLiteralBuffer += currentChar;
            }

            if (!wasTerminated) {
                Error(filePath, inputStream, Span(line, lineStartIndex, characterIndex, 1),
                      "Unexpected end of input.");
            }

            Token token{.type = TokenType::StringLiteral, .line = line, .column = column, .length = stringLiteralBuffer.length()};
            token.value = stringLiteralBuffer;
            tokens.push_back(token);

//            std::cout << "Was string literal: \"" << stringLiteralBuffer << "\"" << std::endl;

            continue;
        }

        std::string errorMessage{ std::format("Unrecognized token: '{}'", currentChar) };
        Error(filePath, inputStream, Span(line, lineStartIndex, characterIndex - 1, 1), errorMessage);
    }

    tokensOut = tokens;
}
