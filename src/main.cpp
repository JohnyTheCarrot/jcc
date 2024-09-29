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

    jcc::Diagnosis::Vec diagnoses;

    try {
        jcc::preprocessor::Preprocessor preprocessor{
                filePath, inputFileStream, diagnoses
        };

        std::for_each(
                preprocessor.begin(), preprocessor.end(),
                [](jcc::tokenizer::Token const &token) {
                    token.DebugPrintTo(std::cout);
                    std::cout << '\n';
                }
        );
    } catch (jcc::FatalCompilerError const &ex) {
        jcc::Diagnosis diag{
                ex.GetSpan(), jcc::Diagnosis::Class::Error, ex.GetKind(),
                ex.GetData1(), ex.GetData2()
        };
        diagnoses.emplace_back(std::move(diag));
    } catch (...) { std::cerr << "An internal compiler error occurred."; }

    for (auto const &diagnosis : diagnoses) {
        diagnosis.Print();
        std::cout << '\n';
    }

    return 0;
}
