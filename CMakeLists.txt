cmake_minimum_required(VERSION 3.22)
project(jcc CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/fmt)
target_include_directories(fmt PRIVATE libs/fmt/include)

set(
        SRC
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
        src/parsing_sema/types/type.h
        src/parsing_sema/types/type.cpp
        src/parsing_sema/parser.h
        src/parsing_sema/ast_node.h
        src/parsing_sema/numeric_constant.h
        src/parsing_sema/numeric_constant.cpp
        src/platform/platform.h
        src/platform/platform.cpp
        src/parsing_sema/constant.h
        src/parsing_sema/constant.cpp
        src/parsing_sema/parser.cpp
        src/parsing_sema/primary_expression.h
        src/parsing_sema/primary_expression.cpp
        src/parsing_sema/postfix_expression.h
        src/parsing_sema/postfix_expression.cpp
        src/parsing_sema/types.h
        src/parsing_sema/types.cpp
        src/parsing_sema/type_name.h
        src/parsing_sema/type_name.cpp
)

add_executable("${PROJECT_NAME}"
        src/main.cpp
        ${SRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

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
        tests/trie.test.cpp
        tests/char_iter.cpp
        tests/tokenizer.test.cpp
        tests/parser.test.cpp
)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

execute_process(COMMAND llvm-config --libs OUTPUT_VARIABLE LIBS)
execute_process(COMMAND llvm-config --system-libs OUTPUT_VARIABLE SYS_LIBS)
execute_process(COMMAND llvm-config --ldflags OUTPUT_VARIABLE LDF)

string(STRIP ${LIBS} LIBS)
string(STRIP ${SYS_LIBS} SYS_LIBS)
string(STRIP ${LDF} LDF)

target_link_libraries(${TEST_PROJECT_NAME} GTest::gtest_main fmt ${LIBS} ${SYS_LIBS} ${LDF})
target_link_libraries(${PROJECT_NAME} GTest::gtest_main fmt ${LIBS} ${SYS_LIBS} ${LDF})
target_include_directories(${TEST_PROJECT_NAME} PRIVATE src libs)

include(GoogleTest)
gtest_discover_tests(${TEST_PROJECT_NAME})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")
    set(COMP_FLAGS --pedantic-errors)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMP_FLAGS})
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE ${COMP_FLAGS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "/EHsc /WX /diagnostics:caret")
    set(COMP_FLAGS /W4 /utf-8)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMP_FLAGS})
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE ${COMP_FLAGS})
endif ()
