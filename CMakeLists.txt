cmake_minimum_required(VERSION 3.25)
project(jcc)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/frozen)

add_subdirectory(libs/fmt)

add_executable(jcc src/main.cpp src/tokenizer.cpp src/tokenizer.h libs/magic_enum/magic_enum.hpp src/reporting.h src/Span.h src/Span.cpp src/reporting.cpp src/parsing/Parser.cpp src/parsing/Parser.h src/parsing/ASTNode.cpp src/parsing/ASTNode.h src/parsing/ParserRuleBuilder.cpp src/parsing/ParserRuleBuilder.h)

target_link_libraries(jcc PUBLIC frozen fmt)
