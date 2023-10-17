cmake_minimum_required(VERSION 3.20)
project(dorset)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 20)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get the latest commit hash
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

add_executable(dorset 
    src/main.cpp
    src/error.cpp
    src/lexer.cpp
    src/token.cpp
    src/ast.cpp
    src/astbuilder.cpp
    src/outpututils.cpp
    src/expressionbuilder.cpp
    src/cli.cpp
)


llvm_map_components_to_libnames(llvm_libs support core irreader orcjit native)

target_link_libraries(dorset ${llvm_libs})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "-DGIT_BRANCH=\"${GIT_BRANCH}\"")

install(TARGETS dorset PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dorset-lang)
