//
// Created by tuurm on 03/07/2023.
//

#include "tokenizer.h"
#include "../libs/magic_enum/magic_enum.hpp"
#include "Span.h"
#include "reporting.h"
#include <cassert>
#include <fmt/format.h>
#include <istream>

size_t Tokenizer::TokenizeNormalToken() {
	// TODO: identifiers starting with a keyword are falsely detected as the keyword followed by an identifier
	const std::streamoff inputStreamPos{this->inputStream.tellg()};
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
	const size_t currentLine{this->line};
	const size_t currentLineStartIndex{this->lineStartIndex};
	const size_t currentCharIndex{GetCharIndex()};

	while (GetNextChar()) {
		if (isalnum(this->currentChar) || this->currentChar == '_') {
			identifierBuffer += this->currentChar;

			continue;
		}

		this->inputStream.putback(this->currentChar);
		break;
	}

	if (identifierBuffer.length() > 0) {
		const Token token{
				TokenType::Identifier,
				Span(currentLine, currentLineStartIndex, currentCharIndex, identifierBuffer.length(),
					 this->inputStream),
				identifierBuffer};
		tokensOut.push_back(token);

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
				  Span(this->line, this->lineStartIndex, GetCharIndex(), 1, this->inputStream), "Unexpected newline.");
		}

		if (this->currentChar == '"') {
			wasTerminated = true;
			break;
		}

		stringLiteralBuffer += this->currentChar;
	}

	if (!wasTerminated) {
		Error(this->filePath, this->inputStream,
			  Span(this->line, this->lineStartIndex, GetCharIndex(), 1, this->inputStream), "Unexpected end of input.");
	}

	tokensOut.emplace_back(
			TokenType::StringLiteral, SpanFromCurrent(stringLiteralBuffer.length()), stringLiteralBuffer
	);
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

	bool hadApostrophe{false};

	if (digit == '\'') {
		hadApostrophe = true;
		if (!GetNextChar() || !GetNextChar())
			return BaseConvertResult::UnexpectedEndOfInput;

		digit = this->currentChar;
	}

	BaseConvertResult convertResult;
	if ((convertResult = ConvertToBase(base, digit, digitOut)) != BaseConvertResult::Success) {
		if (hadApostrophe)
			Error(this->filePath, this->inputStream, SpanFromCurrent(1), "Expected digit after '");

		return convertResult;
	}

	this->currentChar = digit;
	GetNextChar();
	return BaseConvertResult::Success;
}

IntLiteralSuffix
Tokenizer::GetIntLiteralSuffix(IntLiteralSuffix previousSuffix, bool &isSigned_Out, IntegerLiteralType &typeOut) {
	const std::string errorString{"Invalid integer literal suffix."};

	if (ConsumeIfNextChar('u')) {
		if (previousSuffix == IntLiteralSuffix::Unsigned)
			Error(this->filePath, this->inputStream, SpanFromCurrent(1), errorString);

		isSigned_Out = true;
		return IntLiteralSuffix::Unsigned;
	}

	if (previousSuffix == IntLiteralSuffix::Long || previousSuffix == IntLiteralSuffix::LongLong)
		Error(this->filePath, this->inputStream, SpanFromCurrent(1), errorString);

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
		Error(this->filePath, this->inputStream, SpanFromCurrent(1), errorString);
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

	const IntLiteralSuffix suffixOne{GetIntLiteralSuffix(IntLiteralSuffix::None, isUnsigned, type)};
	if (suffixOne != IntLiteralSuffix::None) {
		GetIntLiteralSuffix(suffixOne, isUnsigned, type);
	}

	const size_t amountOfDigits{integerLiteralDigits.size()};
	if (convertResult == BaseConvertResult::DigitNotInBase) {
		Error(this->filePath, this->inputStream,
			  Span(this->line, this->lineStartIndex, GetCharIndex() + 1, 1, this->inputStream),
			  fmt::format("Invalid digit for base {}.", base));
	}

	IntegerTokenValue integerLiteralValue{0};
	for (size_t index{0}; index < amountOfDigits; ++index) {
		integerLiteralValue += integerLiteralDigits[index] *
							   static_cast<IntegerTokenValue>(
									   pow(base, static_cast<double>(amountOfDigits) - static_cast<double>(index) - 1)
							   );
	}

	const IntegerLiteralTokenValue value{integerLiteralValue, isUnsigned, type};
	tokensOut.emplace_back(TokenType::IntegerLiteral, SpanFromCurrent(amountOfDigits), value);
}

bool Tokenizer::TokenizeConstant(TokenList &tokensOut) {
	if (isdigit(this->currentChar) || this->currentChar == '\'') {
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
			this->lineStartIndex = GetCharIndex() + 1;
			this->currentToken = std::nullopt;
			continue;
		}

		// Normal tokens
		const size_t foundTokenLength{TokenizeNormalToken()};

		if (this->currentToken.has_value()) {
			const Token token{this->currentToken.value(), SpanFromCurrent(foundTokenLength)};
			tokensOut.push_back(token);

			continue;
		}

		// Identifiers
		if (isalpha(this->currentChar) || this->currentChar == '_') {
			TokenizeIdentifier(tokensOut);

			continue;
		}

		// String literals
		const bool didMatch{TokenizeConstant(tokensOut)};
		if (didMatch) {
			continue;
		}

		const std::string errorMessage{fmt::format("Unrecognized value: '{}'", this->currentChar)};
		Error(filePath, inputStream, SpanFromCurrent(), errorMessage);
	}
}

bool Tokenizer::GetNextChar() {
	const bool readResult{this->inputStream.get(this->currentChar)};

	if (!readResult) {
		return false;
	}

	if (this->currentChar == '\n') {
		++this->line;
		this->lineStartIndex = GetCharIndex() + 1;
	}

	return true;
}

bool Tokenizer::PeekNextChar(char &out) {
	const char nextChar{static_cast<char>(this->inputStream.peek())};
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
	const int charIndex{static_cast<int>(this->inputStream.tellg()) - 1};

	return std::max(0, charIndex);
}

size_t Tokenizer::GetColumnIndex() const { return GetCharIndex() - this->lineStartIndex; }

Span Tokenizer::SpanFromCurrent(size_t length) const {
	return {this->line, this->lineStartIndex, GetCharIndex(), length, this->inputStream};
}
