cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(FooModule LANGUAGES CXX)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(NAME foo SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# ---- Create standalone executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

add_executable(FooModule ${sources})

set_target_properties(FooModule PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "FooModule")

target_link_libraries(FooModule foo::foo)
