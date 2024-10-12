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

    jcc::Diagnosis::Vec diagnoses;

    try {
        jcc::preprocessor::Preprocessor preprocessor{filePath, diagnoses};

        std::for_each(
                preprocessor.begin(), preprocessor.end(),
                [](jcc::tokenizer::Token const &token) {
                    token.DebugPrintTo(std::cout);
                    std::cout << std::endl;
                }
        );
    } catch (jcc::FatalCompilerError const &ex) {
        diagnoses.emplace_back(ex.GetDiagnosis());
    } catch (...) { std::cerr << "An internal compiler error occurred."; }

    for (auto const &diagnosis : diagnoses) {
        std::cout << diagnosis.GetMessage() << '\n';
    }

    return 0;
}
