#include <algorithm>// for for_each
#include <cstdlib>  // for exit
#include <iostream> // for basic_ostream, char_traits
#include <string>   // for allocator, operator<<, string
#include <vector>   // for vector

#include "misc/Diagnosis.h"           // for Diagnosis, FatalCompilerError
#include "preprocessor/preprocessor.h"// for Preprocessor
#include "tokenizer/token.h"          // for Token

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
