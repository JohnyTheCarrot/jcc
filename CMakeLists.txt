cmake_minimum_required(VERSION 3.22)
project(jcc CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/frozen)

add_subdirectory(libs/fmt)
target_include_directories(fmt PRIVATE libs/fmt/include)

file(GLOB_RECURSE DIR_SRC src/*.cpp)
add_executable("${PROJECT_NAME}" ${DIR_SRC})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_link_libraries(jcc PUBLIC frozen fmt)
