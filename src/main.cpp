#include "misc/Preprocessor.h"
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

	std::string filePath{args[1]};

	IFStream inputFileStream{filePath};
	if (!inputFileStream) {
		std::cerr << "Couldn't open input file: " << strerror(errno) << std::endl;
		exit(1);
	}

	using namespace jcc;

	Diagnosis::Vec diagnoses;
	Preprocessor   preprocessor{std::move(filePath), inputFileStream, diagnoses};

	while (true) {
		const Tokenizer::Token token{preprocessor()};
		std::cout << token << '\n';

		if (std::holds_alternative<Tokenizer::SpecialPurpose>(token.m_Value)) {
			if (const auto value{std::get<Tokenizer::SpecialPurpose>(token.m_Value)};
			    value == Tokenizer::SpecialPurpose::Error || value == Tokenizer::SpecialPurpose::EndOfFile)
				break;
		}
	}

	for (const auto &diagnosis: diagnoses) {
		diagnosis.Print();
		std::cout << '\n';
	}

	return 0;
}
