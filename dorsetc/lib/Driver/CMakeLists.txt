add_library(dorsetDriver STATIC
    CLI.cpp
)

target_compile_definitions(dorsetDriver PRIVATE "-DDORSET_OBJECT_COMPILER=\"${DORSET_OBJECT_COMPILER}\"")

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

target_link_libraries(dorsetDriver dorsetLexicalAnalysis)
target_link_libraries(dorsetDriver dorsetUtils)
target_link_libraries(dorsetDriver dorsetAST)
target_link_libraries(dorsetDriver dorsetBuilder)

target_include_directories(dorsetDriver PRIVATE ../../include)

install(TARGETS dorsetDriver DESTINATION lib)