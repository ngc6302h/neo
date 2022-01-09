cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -fsanitize=address -fsanitize=undefined")
add_compile_options(-fno-exceptions -Wall -Wextra -fdiagnostics-color -Werror -pthread)

project(neo CXX)

include_directories(${CMAKE_SOURCE_DIR})

option(ENABLE_DEBUG_ASSERTS "Defines #DEBUG" ON)
option(VERBOSE_ASSERTS "Print backtraces when an assertion fails" ON)

if(ENABLE_DEBUG_ASSERTS)
    add_compile_definitions(DEBUG_ASSERTS)
    if(VERBOSE_ASSERTS)
        add_compile_definitions(VERBOSE_ASSERTS)
    endif()
endif()

enable_testing()

add_subdirectory(tests)
