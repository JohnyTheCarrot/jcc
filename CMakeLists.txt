cmake_minimum_required(VERSION 3.22)
project(jcc CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/fmt)
target_include_directories(fmt PRIVATE libs/fmt/include)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_library(
        jcc_core STATIC
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
        src/parsing/types/type.h
        src/parsing/types/type.cpp
        src/parsing/parser.h
        src/parsing/ast_node.h
        src/parsing/numeric_constant.h
        src/parsing/numeric_constant.cpp
        src/parsing/constant.h
        src/parsing/constant.cpp
        src/parsing/parser.cpp
        src/parsing/primary_expression.h
        src/parsing/postfix_expression.h
        src/parsing/types.h
        src/parsing/types.cpp
        src/parsing/type_name.h
        src/parsing/type_name.cpp
        src/parsing/ast_node.cpp
        src/parsing/multiplicative_expression.cpp
        src/parsing/multiplicative_expression.h
        src/visitors/expression_codegen.cpp
        src/parsing/additive_expression.cpp
        src/parsing/additive_expression.h
        src/parsing/expression.h
        src/parsing/primary_expression.cpp
        src/parsing/expression.cpp
        src/parsing/shift_expression.cpp
        src/parsing/shift_expression.h
        src/parsing/binary_expression.h
        src/visitors/sema.cpp
        src/visitors/sema.h
        src/diagnostics/diagnostics.cpp
        src/diagnostics/diagnostics.h
        src/diagnostics/variants/sema/binary_operands_wrong_types.cpp
        src/diagnostics/variants/sema/binary_operands_wrong_types.hpp
        src/diagnostics/source.h
        src/diagnostics/source.cpp
        src/diagnostics/variants/tk_pp/untermed_string.cpp
        src/diagnostics/variants/tk_pp/untermed_string.h
        src/diagnostics/variants/tk_pp/untermed_char.cpp
        src/diagnostics/variants/tk_pp/untermed_char.h
        src/diagnostics/variants/tk_pp/char_const_empty.cpp
        src/diagnostics/variants/tk_pp/char_const_empty.h
        src/diagnostics/variants/tk_pp/hex_escape_empty.cpp
        src/diagnostics/variants/tk_pp/hex_escape_empty.hpp
        src/diagnostics/variants/tk_pp/diagnostic_directive_no_tokens.cpp
        src/diagnostics/variants/tk_pp/diagnostic_directive_no_tokens.hpp
        src/diagnostics/variants/tk_pp/custom_diagnostic.cpp
        src/diagnostics/variants/tk_pp/custom_diagnostic.hpp
        src/diagnostics/variants/tk_pp/utf8_too_many_chars.cpp
        src/diagnostics/variants/tk_pp/utf8_too_many_chars.hpp
        src/diagnostics/variants/tk_pp/escape_seq_too_large.cpp
        src/diagnostics/variants/tk_pp/escape_seq_too_large.hpp
        src/diagnostics/variants/tk_pp/multi_byte_char_impl_def.cpp
        src/diagnostics/variants/tk_pp/multi_byte_char_impl_def.hpp
        src/diagnostics/variants/tk_pp/char_const_gt_4_chars.cpp
        src/diagnostics/variants/tk_pp/char_const_gt_4_chars.hpp
        src/diagnostics/variants/tk_pp/unknown_escape_seq.cpp
        src/diagnostics/variants/tk_pp/unknown_escape_seq.hpp
        src/diagnostics/variants/todo.cpp
        src/diagnostics/variants/todo.hpp
        src/diagnostics/variants/tk_pp/pp_conditional_not_terminated.cpp
        src/diagnostics/variants/tk_pp/pp_conditional_not_terminated.hpp
        src/diagnostics/variants/tk_pp/pp_conditional_expected_ident.cpp
        src/diagnostics/variants/tk_pp/pp_conditional_expected_ident.hpp
        src/diagnostics/variants/tk_pp/pp_number_invalid.cpp
        src/diagnostics/variants/tk_pp/pp_number_invalid.hpp
        src/diagnostics/variants/tk_pp/orphaned_endif.cpp
        src/diagnostics/variants/tk_pp/orphaned_endif.hpp
        src/diagnostics/variants/tk_pp/illegal_macro_redef.cpp
        src/diagnostics/variants/tk_pp/illegal_macro_redef.hpp
        src/diagnostics/variants/tk_pp/escape_without_newline.cpp
        src/diagnostics/variants/tk_pp/escape_without_newline.hpp
        src/diagnostics/variants/visitors/diagnostics_visitor.hpp
        src/diagnostics/variants/visitors/mjolnir_visitor.cpp
        src/diagnostics/variants/visitors/mjolnir_visitor.hpp
        src/diagnostics/variants/visitors/diagnostics_visitor.cpp
        src/diagnostics/variants/parsing/expected_expression.cpp
        src/diagnostics/variants/parsing/expected_expression.hpp
        src/diagnostics/variants/unexpected_eof.cpp
        src/diagnostics/variants/unexpected_eof.hpp
        src/diagnostics/variants/tk_pp/unexpected_char.cpp
        src/diagnostics/variants/tk_pp/unexpected_char.hpp
        src/diagnostics/variants/tk_pp/include_open_failed.cpp
        src/diagnostics/variants/tk_pp/include_open_failed.hpp
        src/diagnostics/variants/tk_pp/include_expected_header_name.cpp
        src/diagnostics/variants/tk_pp/include_expected_header_name.hpp
        src/diagnostics/variants/tk_pp/directive_expected_newline.cpp
        src/diagnostics/variants/tk_pp/directive_expected_newline.hpp
        src/diagnostics/variants/tk_pp/undef_expected_ident.cpp
        src/diagnostics/variants/tk_pp/undef_expected_ident.hpp
        src/diagnostics/variants/tk_pp/invalid_macro_param.cpp
        src/diagnostics/variants/tk_pp/invalid_macro_param.hpp
        src/diagnostics/variants/tk_pp/macro_name_not_ident.cpp
        src/diagnostics/variants/tk_pp/macro_name_not_ident.hpp
        src/diagnostics/variants/tk_pp/unexpected_else.cpp
        src/diagnostics/variants/tk_pp/unexpected_else.hpp
        src/diagnostics/variants/tk_pp/else_without_if.cpp
        src/diagnostics/variants/tk_pp/else_without_if.hpp
        src/diagnostics/variants/tk_pp/macro_expected_comma_or_rparen.cpp
        src/diagnostics/variants/tk_pp/macro_expected_comma_or_rparen.hpp
        src/diagnostics/variants/tk_pp/macro_ellipsis_not_last.cpp
        src/diagnostics/variants/tk_pp/macro_ellipsis_not_last.hpp
        src/diagnostics/variants/tk_pp/macro_invoc_expected_lparen.cpp
        src/diagnostics/variants/tk_pp/macro_invoc_expected_lparen.hpp
        src/diagnostics/variants/tk_pp/macro_invoc_invalid_num_args.cpp
        src/diagnostics/variants/tk_pp/macro_invoc_invalid_num_args.hpp
        src/diagnostics/variants/parsing/unrecognized_integer_suffix.cpp
        src/diagnostics/variants/parsing/unrecognized_floating_suffix.cpp
        src/diagnostics/variants/parsing/invalid_floating_point_literal.cpp
        src/diagnostics/variants/parsing/invalid_integer_literal.cpp
        src/diagnostics/variants/parsing/basic_syntax_error.cpp
        src/diagnostics/variants/sema/no_compat_int_type.cpp
        src/parsing/cast_expression.cpp
        src/diagnostics/variants/sema/invalid_specifier_qualifier_list.cpp
        src/visitors/expression_type_deduction.cpp
        src/parsing/relational_expression.cpp
        src/parsing/equality_expression.cpp
        src/parsing/bitwise_and_expression.cpp
        src/parsing/bitwise_xor_expression.cpp
        src/parsing/bitwise_xor_expression.hpp
        src/parsing/bitwise_or_expression.cpp
        src/parsing/bitwise_or_expression.hpp
)

add_executable("${PROJECT_NAME}" src/main.cpp)
target_link_libraries("${PROJECT_NAME}" jcc_core)

target_include_directories(${PROJECT_NAME} PRIVATE src)

set(TEST_PROJECT_NAME "jcc-tests")

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

# Mjolnir
FetchContent_Declare(
        mjolnir
        GIT_REPOSITORY https://github.com/JohnyTheCarrot/mjolnir.git
        GIT_TAG fe1e957606e5ea13c898d0c94e181515fe37cba2
)

FetchContent_MakeAvailable(mjolnir)

add_dependencies(jcc_core mjolnir)
target_link_libraries(jcc_core mjolnir)
target_include_directories(jcc_core PUBLIC ${mjolnir_SOURCE_DIR}/include)

add_executable(${TEST_PROJECT_NAME}
        tests/preprocessor.test.cpp
        tests/trie.test.cpp
        tests/char_iter.cpp
        tests/tokenizer.test.cpp
        tests/parser.test.cpp
        tests/type.test.cpp
)
target_link_libraries(${TEST_PROJECT_NAME} jcc_core)

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
target_link_libraries(jcc_core GTest::gtest_main fmt ${LIBS} ${SYS_LIBS} ${LDF})
target_include_directories(${TEST_PROJECT_NAME} PRIVATE src libs)
target_include_directories(jcc_core PRIVATE src libs)

include(GoogleTest)
gtest_discover_tests(${TEST_PROJECT_NAME})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror -Wno-error=switch")
    set(COMP_FLAGS --pedantic-errors)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMP_FLAGS})
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE ${COMP_FLAGS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "/EHsc /WX /diagnostics:caret")
    set(COMP_FLAGS /W4 /utf-8)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMP_FLAGS})
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE ${COMP_FLAGS})
endif ()
