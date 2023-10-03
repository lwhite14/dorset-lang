cmake_minimum_required(VERSION 3.20)
project(dorset)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 20)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(dorset 
    src/main.cpp
    src/error.cpp
    src/lexer.cpp
    src/token.cpp
    src/ast.cpp
    src/astbuilder.cpp
    src/outpututils.cpp
    src/expressionbuilder.cpp
)


llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(dorset ${llvm_libs})
