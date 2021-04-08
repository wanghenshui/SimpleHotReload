cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(FooModuleTests LANGUAGES CXX)

# ---- Options ----

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage("gh:onqtam/doctest#2.4.5")
CPMAddPackage("gh:TheLartians/Format.cmake@1.7.0")

if(TEST_INSTALLED_VERSION)
  find_package(foo REQUIRED)
else()
  CPMAddPackage(NAME foo SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

# ---- Create binary ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
add_executable(FooModuleTests ${sources})
target_link_libraries(FooModuleTests doctest::doctest foo::foo dl)
set_target_properties(FooModuleTests PROPERTIES CXX_STANDARD 17)

# enable compiler warnings
if(NOT TEST_INSTALLED_VERSION)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(foo PUBLIC -Wall -Wpedantic -Wextra -Werror)
  elseif(MSVC)
    message(FATAL_ERROR "not support msvc")
  endif()
endif()

# ---- Add FooModuleTests ----

enable_testing()

# Note: doctest and similar testing frameworks can automatically configure CMake tests. For other
# testing frameworks add the tests target instead: add_test(NAME FooModuleTests COMMAND FooModuleTests)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(FooModuleTests)

# ---- code coverage ----

if(ENABLE_TEST_COVERAGE)
  target_compile_options(foo PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(foo PUBLIC -fprofile-arcs -ftest-coverage)
endif()
