cmake_minimum_required(VERSION 3.22)
project(jcc CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/fmt)
target_include_directories(fmt PRIVATE libs/fmt/include)

set(
        SRC
        src/misc/CharStream.h
        src/misc/CharStream.cpp
        src/misc/Diagnosis.h
        src/misc/Diagnosis.cpp
        src/misc/Preprocessor.h
        src/misc/Preprocessor.cpp
        src/misc/Span.h
        src/misc/Span.cpp
        src/tokenizer/Tokenizer.h
        src/tokenizer/Tokenizer.cpp
        src/misc/Trie.h
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
)

add_executable("${PROJECT_NAME}"
        src/main.cpp
        ${SRC}
)

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
        tests/tokenizer.test.cpp
        tests/trie.test.cpp
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
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "/J /EHsc /diagnostics:caret")
    set(COMP_FLAGS /W4 /utf-8)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMP_FLAGS})
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE ${COMP_FLAGS})
endif ()
