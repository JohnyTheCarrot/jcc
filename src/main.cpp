#include <cstring>
#include <fstream>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

#include "preprocessor/preprocessor.h"

int main(int argCount, char *args[]) {
    if (argCount < 2) {
        std::cerr << "Please specify an input file." << std::endl;
        exit(1);
    }

    std::string filePath{args[1]};

    std::ifstream inputFileStream{filePath};
    if (!inputFileStream) {
        std::cerr << "Couldn't open input file: " << strerror(errno)
                  << std::endl;
        exit(1);
    }

    jcc::Diagnosis::Vec             diagnoses;
    jcc::preprocessor::Preprocessor preprocessor{
            filePath, inputFileStream, diagnoses
    };

    try {
        std::ranges::copy(
                preprocessor,
                std::ostream_iterator<jcc::tokenizer::Token>{std::cout, "\n"}
        );
    } catch (jcc::FatalCompilerError const &ex) {
        jcc::Diagnosis diag{
                ex.GetSpan(), jcc::Diagnosis::Class::Error, ex.GetKind(),
                ex.GetData1(), ex.GetData2()
        };
        diag.Print();
        std::cout << '\n';
    }

    for (auto const &diagnosis : diagnoses) {
        diagnosis.Print();
        std::cout << '\n';
    }

    return 0;
}
