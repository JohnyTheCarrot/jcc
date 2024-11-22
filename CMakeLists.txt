cmake_minimum_required(VERSION 3.22)
project(jcc CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/fmt)
target_include_directories(fmt PRIVATE libs/fmt/include)

set(PARSER_GEN_SRC
        ${CMAKE_SOURCE_DIR}/pre_build/parser_gen/main.cpp
        ${CMAKE_SOURCE_DIR}/pre_build/parser_gen/grammar_parser.cpp
        ${CMAKE_SOURCE_DIR}/pre_build/parser_gen/production.cpp
)

set(PRE_BUILD_DIR ${CMAKE_SOURCE_DIR}/pre_build)
set(GRAMMAR_FILE ${PRE_BUILD_DIR}/grammar.txt)
set(GRAMMAR_OUTPUT_FILE ${CMAKE_BINARY_DIR}/grammar.txt)

# Assuming lr1_gen_exec generates 'parser.cpp' as its output
set(GENERATED_LR_TABLE_CPP ${CMAKE_BINARY_DIR}/gen/lr_table.cpp)
set(GENERATED_LR_TABLE_HPP ${CMAKE_BINARY_DIR}/gen/lr_table.hpp)
set(GENERATED_OUTPUT ${GENERATED_LR_TABLE_CPP} ${GENERATED_LR_TABLE_HPP})

# Step 1: Copy the grammar file only if it has changed
add_custom_command(
        OUTPUT ${GRAMMAR_OUTPUT_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GRAMMAR_FILE} ${GRAMMAR_OUTPUT_FILE}
        DEPENDS ${GRAMMAR_FILE}
        COMMENT "Copying grammar file"
)

# Step 2: Compile lr1_gen.cpp only if grammar.txt or lr1_gen.cpp has changed
add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/lr1_gen_exec
        COMMAND ${CMAKE_CXX_COMPILER} -o "${CMAKE_BINARY_DIR}/lr1_gen_exec" ${PARSER_GEN_SRC} -std=c++20
        COMMENT "Building lr1_gen"
        DEPENDS ${GRAMMAR_OUTPUT_FILE} ${PARSER_GEN_SRC}
)

# Step 3: Run lr1_gen_exec only if the GENERATED_OUTPUT is missing or needs to be regenerated
add_custom_command(
        OUTPUT ${GENERATED_OUTPUT}
        COMMAND ${CMAKE_BINARY_DIR}/lr1_gen_exec ${GENERATED_LR_TABLE_CPP} ${GENERATED_LR_TABLE_HPP}
        DEPENDS ${CMAKE_BINARY_DIR}/lr1_gen_exec ${GRAMMAR_OUTPUT_FILE}
        COMMENT "Running lr1_gen to generate parser.cpp"
)

# Step 4: Create a custom target that depends on the generated output
add_custom_target(run_lr1_gen
        DEPENDS ${GENERATED_OUTPUT}
        COMMENT "Ensuring lr1_gen_exec has run if necessary"
)

add_executable(lr1_gen_exec ${PARSER_GEN_SRC})

set(
        SRC
        src/misc/CharStream.h
        src/misc/CharStream.cpp
        src/misc/Diagnosis.h
        src/misc/Diagnosis.cpp
        src/misc/Span.h
        src/misc/Span.cpp
        src/misc/trie.h
        src/preprocessor/preprocessor.h
        src/preprocessor/preprocessor.cpp
        src/preprocessor/commands/command.h
        src/preprocessor/commands/command.cpp
        src/preprocessor/macro.h
        src/preprocessor/preprocessor_iterator.h
        src/preprocessor/commands/define_command.cpp
        src/preprocessor/preprocessor_iterator.cpp
        src/preprocessor/commands/identifier_command.h
        src/preprocessor/commands/define_command.h
        src/preprocessor/commands/identifier_command.cpp
        src/preprocessor/preprocessor_token.h
        src/preprocessor/preprocessor_token.cpp
        src/tokenizer/tokenizer.h
        src/tokenizer/tokenizer.cpp
        src/tokenizer/token.h
        src/tokenizer/token.cpp
        src/tokenizer/char_iter.h
        src/tokenizer/char_iter.cpp
        src/tokenizer/tokenizer_iterator.cpp
        src/tokenizer/tokenizer_iterator.h
        src/tokenizer/tokens/static_tokens.h
        src/tokenizer/tokens/static_tokens.cpp
        src/tokenizer/tokens/identifiers.h
        src/tokenizer/tokens/identifiers.cpp
        src/tokenizer/tokens/character_constants.h
        src/tokenizer/tokens/character_constants.cpp
        src/tokenizer/tokens/escape_sequences.h
        src/tokenizer/tokens/escape_sequences.cpp
        src/tokenizer/tokens/string_literals.h
        src/tokenizer/tokens/string_literals.cpp
        src/tokenizer/tokens/utils.h
        src/tokenizer/tokens/utils.cpp
        src/tokenizer/tokens/pp_numbers.h
        src/tokenizer/tokens/pp_numbers.cpp
        src/preprocessor/macro.cpp
        src/preprocessor/macro_store.h
        src/preprocessor/macro_store.cpp
        src/preprocessor/commands/warning_command.h
        src/preprocessor/commands/warning_command.cpp
        src/preprocessor/commands/error_command.h
        src/preprocessor/commands/error_command.cpp
        src/preprocessor/commands/include_command.h
        src/preprocessor/commands/include_command.cpp
        src/preprocessor/commands/ifdef_command.h
        src/preprocessor/commands/ifdef_command.cpp
        src/preprocessor/commands/endif_command.h
        src/preprocessor/commands/endif_command.cpp
        src/preprocessor/commands/else_command.h
        src/preprocessor/commands/else_command.cpp
        src/preprocessor/commands/undef_command.h
        src/preprocessor/commands/undef_command.cpp
        src/ir_codegen/ir_gen.cpp
        src/ir_codegen/ir_gen.h
        src/ir_codegen/target/target.h
        src/ir_codegen/target/6502_asm/asm6502.h
        src/ir_codegen/target/6502_asm/asm6502.cpp
        src/ir_codegen/target/register.h
        src/ir_codegen/target/register.cpp
        src/ir_codegen/target/target.cpp
        src/ir_codegen/target/6502_asm/printer.h
        src/ir_codegen/target/6502_asm/printer.cpp
        src/ir_codegen/target/common.h
        src/parsing/parser.h
        src/parsing/parser.cpp
        src/parsing/lrtable_utils.hpp
        ${GENERATED_OUTPUT}
)

add_executable("${PROJECT_NAME}"
        src/main.cpp
        ${SRC}
        #        ${PARSER_GEN_SRC}
)

add_dependencies(${PROJECT_NAME} run_lr1_gen)

target_include_directories(${PROJECT_NAME} PRIVATE src ${CMAKE_BINARY_DIR}/gen)

set(TEST_PROJECT_NAME "jcc-tests")

#if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#endif ()

target_include_directories(${PROJECT_NAME} PRIVATE libs)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(${TEST_PROJECT_NAME} ${SRC}
        tests/preprocessor.test.cpp
        #        tests/tokenizer.old.test.cpp
        tests/trie.test.cpp
        tests/char_iter.cpp
        tests/tokenizer.test.cpp
)

target_link_libraries(${TEST_PROJECT_NAME} GTest::gtest_main fmt)
target_link_libraries(${PROJECT_NAME} GTest::gtest_main fmt)
target_include_directories(${TEST_PROJECT_NAME} PRIVATE src libs)

include(GoogleTest)
gtest_discover_tests(${TEST_PROJECT_NAME})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -funsigned-char")
    set(COMP_FLAGS --pedantic-errors)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMP_FLAGS})
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE ${COMP_FLAGS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -funsigned-char")
    set(COMP_FLAGS --pedantic-errors)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMP_FLAGS})
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE ${COMP_FLAGS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "/J /EHsc /diagnostics:caret")
    set(COMP_FLAGS /W4 /utf-8)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMP_FLAGS})
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE ${COMP_FLAGS})
endif ()
