add_library(dorsetAST STATIC
    AST.cpp
)

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

target_link_libraries(dorsetAST ${llvm_libs})
target_link_libraries(dorsetAST dorsetUtils)

target_include_directories(dorsetAST PRIVATE ../../include)

install(TARGETS dorsetAST DESTINATION lib)