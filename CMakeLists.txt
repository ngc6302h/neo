cmake_minimum_required(VERSION 3.18)
project(neo CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR})

set(DCMAKE_CXX_FLAGS_RELEASE "-O3 -flto")
set(DCMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -flto -DNDEBUG")
set(DCMAKE_CXX_FLAGS_DEBUG "-Og -g -DNDEBUG -DDEBUG -fsanitize=address -fsanitize=undefined -Wall -Wextra -pedantic -Werror")
add_compile_options(-fno-exceptions -Wall -Wextra -pedantic -Werror -pthread)

option(DEFINE_DEBUG "Defines #DEBUG" ON)
if(DEFINE_DEBUG)
    add_compile_definitions(DEBUG)
endif()

enable_testing()

add_subdirectory(tests)