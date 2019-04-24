cmake_minimum_required(VERSION 3.12)

project(math-logic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_compile_definitions(LOCAL)

add_executable(parse full/grammar/grammar.cpp full/parser/parser.cpp full/parser/expression_hash.cpp
        full/parse.cpp)

add_executable(minimize full/grammar/grammar.cpp full/parser/parser.cpp full/parser/expression_hash.cpp
        full/proof/proof_utils.cpp
        full/minimize.cpp)

add_executable(intuitionistic full/grammar/grammar.cpp full/parser/parser.cpp full/parser/expression_hash.cpp
        full/proof/proof_utils.cpp full/proof/proof_translator.hpp
        full/intuitionistic.cpp)

add_executable(prove full/grammar/grammar.cpp full/parser/parser.cpp full/parser/expression_hash.cpp
        full/proof/proof_utils.cpp full/proof/proof_translator.hpp full/proof/proof_creator.hpp
        full/prove.cpp)