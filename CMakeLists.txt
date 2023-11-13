cmake_minimum_required(VERSION 3.20)
project(dorsetc VERSION 0.1.5 DESCRIPTION "Dorset compiler for the Dorset programming language.")

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

add_library(dorsetDriver STATIC
    src/Utils/Error.cpp
    src/Utils/OutputUtils.cpp
    src/LexicalAnalysis/Lexer.cpp
    src/LexicalAnalysis/Token.cpp
    src/Builder/ASTBuilder.cpp
    src/Builder/ExpressionBuilder.cpp
    src/AST/AST.cpp
    src/CLI/CLI.cpp
)

if (WIN32)
    add_executable(${PROJECT_NAME} 
        src/AppEntry.cpp
        branding/icon.o
    )
else()
    add_executable(${PROJECT_NAME} 
        src/AppEntry.cpp
    )
endif()

llvm_map_components_to_libnames(llvm_libs support core irreader orcjit native)

target_link_libraries(dorsetDriver ${llvm_libs})

target_link_libraries(${PROJECT_NAME} dorsetDriver)

target_compile_definitions(dorsetDriver PRIVATE
    "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")

target_compile_definitions(dorsetDriver PRIVATE
    "-DGIT_BRANCH=\"${GIT_BRANCH}\"")

install(TARGETS ${PROJECT_NAME} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS dorsetDriver PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Testing
add_executable(compileTests tests/CompileTests.cpp)
add_executable(benchmarkTests tests/BenchmarkTests.cpp)

target_link_libraries(compileTests dorsetDriver)
target_link_libraries(benchmarkTests dorsetDriver)

enable_testing()
add_test(NAME compileTests COMMAND $<TARGET_FILE:compileTests>)
add_test(NAME benchmarkTests COMMAND $<TARGET_FILE:benchmarkTests>)
file(COPY tests/src DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
