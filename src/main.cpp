#include <algorithm>// for for_each
#include <cstdlib>  // for exit
#include <iostream> // for basic_ostream, char_traits
#include <string>   // for allocator, operator<<, string
#include <vector>   // for vector

#include "codegen/expressions.h"
#include "parsing_sema/additive_expression.h"
#include "parsing_sema/expression.h"
#include "parsing_sema/numeric_constant.h"
#include "parsing_sema/sema_visitor.h"
#include "preprocessor/preprocessor.h"// for Preprocessor

int main(int argCount, char *args[]) {
    if (argCount < 2) {
        std::cerr << "Please specify an input file." << std::endl;
        exit(1);
    }

    std::string const filePath{args[1]};

    auto &compState{jcc::parsing_sema::CompilerState::GetInstance()};

    try {
        jcc::preprocessor::Preprocessor preprocessor{filePath};
        auto const &compilerState{jcc::parsing_sema::CompilerState::GetInstance(
        )};

        auto startIt{preprocessor.begin()};

        auto const constant{
                jcc::parsing_sema::ParseExpression(startIt, preprocessor.end())
        };
        jcc::parsing_sema::SemaVisitor semaVisitor;
        if (constant != nullptr) {
            constant->Accept(&semaVisitor);

            if (!compilerState.HasFatalError()) {
                jcc::codegen::ExpressionCodegenVisitor codegenVisitor;

                constant->Accept(&codegenVisitor);
                auto *value{codegenVisitor.GetValue()};
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
            }
        }
    } catch (jcc::diagnostics::FatalCompilerError const &) {
        std::cerr << "A fatal compiler error occurred." << std::endl;
    } catch (...) { std::cerr << "An internal compiler error occurred."; }

    using DiagnosticsFormat =
            jcc::parsing_sema::CompilerState::DiagnosticsFormat;

    compState.PrintDiagnostics(DiagnosticsFormat::Fancy);

    return 0;
}
