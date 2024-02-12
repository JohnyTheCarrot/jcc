cmake_minimum_required(VERSION 3.22)
project(jcc CXX)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/fmt)
target_include_directories(fmt PRIVATE libs/fmt/include)

file(GLOB_RECURSE DIR_SRC src/*.cpp)
add_executable("${PROJECT_NAME}" ${DIR_SRC})

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

file(GLOB_RECURSE TEST_SRC tests/*.cpp)
file(GLOB_RECURSE SRC_NO_MAIN src/parsing/*.cpp src/misc/*.cpp)
add_executable(${TEST_PROJECT_NAME} ${TEST_SRC} ${SRC_NO_MAIN})

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
    set(CMAKE_CXX_FLAGS "/J /EHsc")
    set(COMP_FLAGS /Wall /utf-8)
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMP_FLAGS})
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE ${COMP_FLAGS})
endif ()
