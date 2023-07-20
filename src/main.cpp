#include <iostream>
#include <fstream>
#include <cstring>
#include "tokenizer.h"
#include "../libs/magic_enum/magic_enum.hpp"

int main(int argCount, char *args[]) {
    if (argCount < 2) {
        std::cerr << "Please specify an input file." << std::endl;
        exit(1);
    }

    std::string filePath{args[1] };

    std::ifstream inputFileStream{filePath };
    if (!inputFileStream)
    {
        std::cerr << "Couldn't open input file: " << strerror(errno) << std::endl;
        exit(1);
    }

    TokenList tokens;
    Tokenizer tokenizer{filePath, inputFileStream, tokens};
    tokenizer.Tokenize();

    return 0;
}
