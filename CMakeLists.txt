cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
add_compile_options(-fno-exceptions -Wall -Wextra -Werror -fdiagnostics-color=always -fconcepts-diagnostics-depth=2 -pthread)

project(neo CXX)

include_directories(${CMAKE_SOURCE_DIR})

option(ENABLE_DEBUG_ASSERTS "Defines #DEBUG_ASSERTS" ON)
option(VERBOSE_ASSERTS "Print backtraces when an assertion fails" ON)

if(ENABLE_DEBUG_ASSERTS)
    add_compile_definitions(DEBUG_ASSERTS=1)
    if(VERBOSE_ASSERTS)
        add_compile_definitions(VERBOSE_ASSERTS)
    endif()
else()
    add_compile_definitions(DEBUG_ASSERTS=0)
endif()

enable_testing()

add_subdirectory(tests)

file(GLOB neo_headers *.h)
add_library(neo INTERFACE ${neo_headers})
