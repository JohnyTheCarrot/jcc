#include "misc/Tokenizer.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

int main(int argCount, char *args[]) {
	if (argCount < 2) {
		std::cerr << "Please specify an input file." << std::endl;
		exit(1);
	}

	const std::string filePath{args[1]};

	IFStream inputFileStream{filePath};
	if (!inputFileStream) {
		std::cerr << "Couldn't open input file: " << strerror(errno) << std::endl;
		exit(1);
	}

	Diagnosis::Vec diagnoses;
	Tokenizer      tokenizer{inputFileStream, diagnoses};

	return 0;
}
