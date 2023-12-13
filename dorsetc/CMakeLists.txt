cmake_minimum_required(VERSION 3.23)
project(dorsetc VERSION 0.1.5 DESCRIPTION "Dorset compiler for the Dorset programming language.")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 20)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(DORSET_OBJECT_COMPILER "gcc" CACHE STRING "The binary responsible for compiling the final object file.")
message(STATUS "Object Compiler: ${DORSET_OBJECT_COMPILER}")

# Get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest commit hash
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_library(dorsetDriver STATIC
    lib/dorsetDriver/Utils/Error.cpp
    lib/dorsetDriver/Utils/OutputUtils.cpp
    lib/dorsetDriver/LexicalAnalysis/Lexer.cpp
    lib/dorsetDriver/LexicalAnalysis/Token.cpp
    lib/dorsetDriver/Builder/ASTBuilder.cpp
    lib/dorsetDriver/Builder/ExpressionBuilder.cpp
    lib/dorsetDriver/AST/AST.cpp
    lib/dorsetDriver/CLI/CLI.cpp
)

if (WIN32)
    add_executable(${PROJECT_NAME} 
        bin/dorsetc/AppEntry.cpp
        resources/icon.o
    )
else()
    add_executable(${PROJECT_NAME} 
        bin/dorsetc/AppEntry.cpp
    )
endif()

llvm_map_components_to_libnames(llvm_libs 
    Support 
    Core 
    IRReader 
    OrcJIT 
    native
    Target

    AArch64
    AMDGPU
    ARM
    AVR
    BPF
    Hexagon
    Lanai
    LoongArch
    Mips
    MSP430
    NVPTX
    PowerPC
    RISCV
    Sparc
    SystemZ
    VE
    WebAssembly
    XCore
    M68k
    Xtensa
)

target_compile_definitions(dorsetDriver PRIVATE "-DDORSET_OBJECT_COMPILER=\"${DORSET_OBJECT_COMPILER}\"")
target_compile_definitions(dorsetDriver PRIVATE "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")
target_compile_definitions(dorsetDriver PRIVATE "-DGIT_BRANCH=\"${GIT_BRANCH}\"")

target_sources(dorsetDriver
    PUBLIC FILE_SET HEADERS 
    BASE_DIRS include
    FILES 
    include/dorsetDriver/Utils/Error.h
    include/dorsetDriver/Utils/OutputUtils.h
    include/dorsetDriver/LexicalAnalysis/Lexer.h
    include/dorsetDriver/LexicalAnalysis/Token.h
    include/dorsetDriver/Builder/ASTBuilder.h
    include/dorsetDriver/Builder/ExpressionBuilder.h
    include/dorsetDriver/AST/AST.h
    include/dorsetDriver/CLI/CLI.h
)

install(TARGETS ${PROJECT_NAME} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS dorsetDriver FILE_SET HEADERS)

add_executable(compileTest test/bin/compileTest/CompileTest.cpp)
add_executable(benchmarkTest test/bin/benchmarkTest/BenchmarkTest.cpp)

enable_testing()
add_test(NAME compileTest COMMAND $<TARGET_FILE:compileTest>)
add_test(NAME benchmarkTest COMMAND $<TARGET_FILE:benchmarkTest>)
file(COPY test/src DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


target_include_directories(dorsetDriver     PRIVATE include)
target_include_directories(dorsetDriver     PRIVATE deps)

target_include_directories(${PROJECT_NAME}  PRIVATE include)

target_include_directories(compileTest      PRIVATE include)
target_include_directories(compileTest      PRIVATE deps)

target_include_directories(benchmarkTest    PRIVATE include)
target_include_directories(benchmarkTest    PRIVATE deps)


target_link_libraries(dorsetDriver      ${llvm_libs})

target_link_libraries(${PROJECT_NAME}   dorsetDriver)
target_link_libraries(compileTest       dorsetDriver)
target_link_libraries(benchmarkTest     dorsetDriver)
