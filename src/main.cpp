#include "../libs/magic_enum/magic_enum.hpp"
#include "parsing/Parser.h"
#include "tokenizer.h"
#include <cstring>
#include <fstream>
#include <iostream>

int main(int argCount, char *args[]) {
	if (argCount < 2) {
		std::cerr << "Please specify an input file." << std::endl;
		exit(1);
	}

	std::string filePath{args[1]};

	std::ifstream inputFileStream{filePath};
	if (!inputFileStream) {
		std::cerr << "Couldn't open input file: " << strerror(errno) << std::endl;
		exit(1);
	}

	TokenList tokens;
	Tokenizer tokenizer{filePath, inputFileStream};
	tokenizer.Tokenize(tokens);

	std::cout << "Tokens: [";
	for (const Token &token: tokens) { std::cout << magic_enum::enum_name(token._type) << ", "; }

	std::cout << ']' << std::endl << std::endl;

	std::cout << "Parse result:" << std::endl;

	Parser parser{std::move(tokens), filePath, inputFileStream};
	parser.Parse();

	return 0;
}
