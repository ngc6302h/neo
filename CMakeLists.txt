cmake_minimum_required(VERSION 3.19)
project(neo CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DCMAKE_CXX_FLAGS_RELEASE "-O3 -flto")
set(DCMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -flto -DNDEBUG")
set(DCMAKE_CXX_FLAGS_DEBUG "-Og -g -DNDEBUG -DDEBUG -fsanitize=address -fsanitize=undefined -Wall -Wextra -pedantic -Werror")
add_compile_options(-Wall -Wextra -pedantic -Werror)

option(DEFINE_DEBUG "Defines #DEBUG" ON)
if(DEFINE_DEBUG)
    add_compile_definitions(DEBUG)
endif()

#just so that clion code analysis works we make a dummy target
add_executable(neo Test.cpp AsciiString.h Assert.h Hashmap.h IterableUtil.h Iterator.h New.h Optional.h
        Queue.h Span.h Stack.h String.h Types.h TypeTraits.h Variant.h Vector.h)
set_target_properties(neo PROPERTIES LINKER_LANGUAGE CXX)
