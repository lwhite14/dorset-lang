cmake_minimum_required(VERSION 3.23)
project(dorsetc VERSION 0.2.1 DESCRIPTION "Dorset compiler for the Dorset programming language.")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 20)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(DORSET_OBJECT_COMPILER "gcc" CACHE STRING "The binary responsible for compiling the final object file.")
message(STATUS "Object Compiler: ${DORSET_OBJECT_COMPILER}")

add_subdirectory(lib)
add_subdirectory(bin)
add_subdirectory(include)
add_subdirectory(test)
