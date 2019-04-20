cmake_minimum_required(VERSION 3.12)

project(01-lab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_compile_definitions(LOCAL)
add_executable(full full/grammar/grammar.cpp full/parser/parser.cpp
        full/parser/expression_hash.cpp full/proof/proof_utils.cpp full/minimize.cpp)