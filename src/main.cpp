#include "misc/Tokenizer.h"
#include "misc/preprocessor/preprocessor.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

int main(int argCount, char *args[]) {
	if (argCount < 2) {
		std::cerr << "Please specify an input file." << std::endl;
		exit(1);
	}

	std::string filePath{args[1]};

	IFStream inputFileStream{filePath};
	if (!inputFileStream) {
		std::cerr << "Couldn't open input file: " << strerror(errno) << std::endl;
		exit(1);
	}

	using namespace jcc;

	Diagnosis::Vec             diagnoses;
	preprocessor::Preprocessor preprocessor{filePath, inputFileStream, diagnoses};

	std::ranges::copy(preprocessor, std::ostream_iterator<Tokenizer::Token>{std::cout, "\n"});

	for (auto const &diagnosis: diagnoses) {
		diagnosis.Print();
		std::cout << '\n';
	}

	return 0;
}
