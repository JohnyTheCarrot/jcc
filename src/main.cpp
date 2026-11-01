#include <algorithm>// for for_each
#include <cstdlib>  // for exit
#include <diagnostics/variants/parsing/basic_syntax_error.hpp>
#include <iostream>// for basic_ostream, char_traits
#include <iterator>
#include <string>  // for allocator, operator<<, string
#include <vector>  // for vector
#include <ranges>
#include <ostream>

#include "codegen_llvm/codegen_backend.h"
#include "parsing/additive_expression.h"
#include "parsing/expression.h"
#include "parsing/numeric_constant.h"
#include "parsing/parser.h"
#include "preprocessor/preprocessor.h"// for Preprocessor
#include "tokenizer/token.h"
#include "visitors/expression_type_deduction.hpp"
#include "visitors/sema.h"

int main(int argCount, char *args[]) {
    if (argCount < 2) {
        std::cerr << "Please specify an input file." << std::endl;
        exit(1);
    }

    std::string const filePath{args[1]};

    auto &compState{jcc::parsing::CompilerState::GetInstance()};

    try {
        jcc::preprocessor::Preprocessor preprocessor{filePath};
        // auto const &compilerState{jcc::parsing::CompilerState::GetInstance()};

        // auto startIt{preprocessor.begin()};

        std::ranges::copy(preprocessor, std::ostream_iterator<jcc::tokenizer::Token>(std::cout, " "));
        std::cout << "\n\n-- end of preprocessor output" << std::endl;

//         auto const constant{
//                 jcc::parsing::ParseExpression(startIt, preprocessor.end())
//         };
//         if (startIt != preprocessor.end()) {
//             jcc::parsing::CompilerState::GetInstance()
//                     .EmplaceDiagnostic<jcc::diagnostics::BasicSyntaxError>(
//                             startIt->m_Span.m_Source, startIt->m_Span.m_Span,
//                             "Expected end of file, but found: " +
//                                     startIt->ToString()
//                     );
//         }
//         if (constant != nullptr) {
//             jcc::visitors::ExpressionTypeDeductionVisitor typeDeductionVisitor;
//             constant->AcceptOnExpression(&typeDeductionVisitor);
//             jcc::visitors::SemaVisitor semaVisitor;
//             constant->AcceptOnExpression(&semaVisitor);
//
//             if (!compilerState.HasFatalError()) {
//                 jcc::codegen_llvm::LLVMCodegenBackend backend;
//                 backend.EmitExpression(filePath, constant.get());
//             }
//         }
    } catch (jcc::diagnostics::FatalCompilerError const &) {
        std::cerr << "A fatal compiler error occurred." << std::endl;
    } catch (std::exception const &e) {
        std::cerr << "An exception occurred: " << e.what() << std::endl;
    } catch (...) { std::cerr << "An unknown internal compiler error occurred." << std::endl; }

    using DiagnosticsFormat = jcc::parsing::CompilerState::DiagnosticsFormat;

    compState.PrintDiagnostics(DiagnosticsFormat::Fancy);

    return 0;
}
