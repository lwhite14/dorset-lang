add_library(dorsetDriver STATIC
    Utils/Error.cpp
    Utils/OutputUtils.cpp
    Utils/Version.cpp
    LexicalAnalysis/Lexer.cpp
    LexicalAnalysis/Token.cpp
    Builder/ASTBuilder.cpp
    Builder/ExpressionBuilder.cpp
    AST/AST.cpp
    CLI/CLI.cpp
)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

target_compile_definitions(dorsetDriver PRIVATE "-DDORSET_OBJECT_COMPILER=\"${DORSET_OBJECT_COMPILER}\"")
target_compile_definitions(dorsetDriver PRIVATE "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")
target_compile_definitions(dorsetDriver PRIVATE "-DGIT_BRANCH=\"${GIT_BRANCH}\"")

target_include_directories(dorsetDriver PRIVATE ../../include)
target_include_directories(dorsetDriver PRIVATE ../../deps)

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

target_link_libraries(dorsetDriver ${llvm_libs})

install(TARGETS dorsetDriver DESTINATION lib)