cmake_minimum_required(VERSION 3.20)

project(summit)

set(CMAKE_CXX_STANDARD 20)

add_executable(summit 
    src/main.cpp
    src/error.cpp
    src/scanner.cpp
    src/token.cpp
    src/ast.cpp
    src/parser.cpp
    src/outpututils.cpp
)
