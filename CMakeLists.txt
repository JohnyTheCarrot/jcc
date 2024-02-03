cmake_minimum_required(VERSION 3.22)
project(jcc CXX)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/frozen)

add_subdirectory(libs/fmt)
target_include_directories(fmt PRIVATE libs/fmt/include)

file(GLOB_RECURSE DIR_SRC src/*.cpp)
add_executable("${PROJECT_NAME}" ${DIR_SRC})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
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

file(GLOB_RECURSE TEST_SRC tests/*.cpp)
file(GLOB_RECURSE SRC_NO_MAIN src/parsing/*.cpp src/misc/*.cpp)
add_executable(jcc_tests ${TEST_SRC} ${SRC_NO_MAIN})

target_link_libraries(jcc_tests GTest::gtest_main frozen fmt)
target_link_libraries(${PROJECT_NAME} GTest::gtest_main frozen fmt)
target_include_directories(jcc_tests PRIVATE src libs)

include(GoogleTest)
gtest_discover_tests(jcc_tests)
