#include <algorithm>// for for_each
#include <cstdlib>  // for exit
#include <iostream> // for basic_ostream, char_traits
#include <string>   // for allocator, operator<<, string
#include <vector>   // for vector

#include "misc/Diagnosis.h"// for Diagnosis, FatalCompilerError
#include "parsing_sema/numeric_constant.h"
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

        auto token{*preprocessor.begin()};

        auto  constant{jcc::parsing_sema::ParseNumericConstant(token)};
        auto *value{constant->Codegen()};
        auto &compState{jcc::parsing_sema::CompilerState::GetInstance()};
        auto &context{compState.GetContext()};
        auto &builder{compState.GetBuilder()};

        llvm::Module module(filePath, context);

        llvm::FunctionType *fnType{
                llvm::FunctionType::get(value->getType(), false)
        };

        llvm::Function *fn{llvm::Function::Create(
                fnType, llvm::Function::ExternalLinkage, "test", module
        )};

        llvm::BasicBlock *entry =
                llvm::BasicBlock::Create(context, "entry", fn);
        builder.SetInsertPoint(entry);
        builder.CreateRet(value);

        module.print(llvm::outs(), nullptr);

        // std::for_each(
        //         preprocessor.begin(), preprocessor.end(),
        //         [](jcc::tokenizer::Token const &token) {
        //             token.DebugPrintTo(std::cout);
        //             std::cout << std::endl;
        //         }
        // );
    } catch (jcc::FatalCompilerError const &ex) {
        diagnoses.emplace_back(ex.GetDiagnosis());
    } catch (...) { std::cerr << "An internal compiler error occurred."; }

    for (auto const &diagnosis : diagnoses) {
        std::cout << diagnosis.GetMessage() << '\n';
    }

    return 0;
}
