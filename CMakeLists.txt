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


llvm_map_components_to_libnames(llvm_libs
    WindowsManifest
    XRay
    LibDriver
    DlltoolDriver
    Coverage
    LineEditor
    XCoreDisassembler
    XCoreCodeGen
    XCoreDesc
    XCoreInfo
    X86TargetMCA
    X86Disassembler
    X86AsmParser
    X86CodeGen
    X86Desc
    X86Info
    WebAssemblyDisassembler
    WebAssemblyAsmParser
    WebAssemblyCodeGen
    WebAssemblyUtils
    WebAssemblyDesc
    WebAssemblyInfo
    VEDisassembler
    VEAsmParser
    VECodeGen
    VEDesc
    VEInfo
    SystemZDisassembler
    SystemZAsmParser
    SystemZCodeGen
    SystemZDesc
    SystemZInfo
    SparcDisassembler
    SparcAsmParser
    SparcCodeGen
    SparcDesc
    SparcInfo
    RISCVTargetMCA
    RISCVDisassembler
    RISCVAsmParser
    RISCVCodeGen
    RISCVDesc
    RISCVInfo
    PowerPCDisassembler
    PowerPCAsmParser
    PowerPCCodeGen
    PowerPCDesc
    PowerPCInfo
    NVPTXCodeGen
    NVPTXDesc
    NVPTXInfo
    MSP430Disassembler
    MSP430AsmParser
    MSP430CodeGen
    MSP430Desc
    MSP430Info
    MipsDisassembler
    MipsAsmParser
    MipsCodeGen
    MipsDesc
    MipsInfo
    LoongArchDisassembler
    LoongArchAsmParser
    LoongArchCodeGen
    LoongArchDesc
    LoongArchInfo
    LanaiDisassembler
    LanaiCodeGen
    LanaiAsmParser
    LanaiDesc
    LanaiInfo
    HexagonDisassembler
    HexagonCodeGen
    HexagonAsmParser
    HexagonDesc
    HexagonInfo
    BPFDisassembler
    BPFAsmParser
    BPFCodeGen
    BPFDesc
    BPFInfo
    AVRDisassembler
    AVRAsmParser
    AVRCodeGen
    AVRDesc
    AVRInfo
    ARMDisassembler
    ARMAsmParser
    ARMCodeGen
    ARMDesc
    ARMUtils
    ARMInfo
    AMDGPUTargetMCA
    AMDGPUDisassembler
    AMDGPUAsmParser
    AMDGPUCodeGen
    AMDGPUDesc
    AMDGPUUtils
    AMDGPUInfo
    AArch64Disassembler
    AArch64AsmParser
    AArch64CodeGen
    AArch64Desc
    AArch64Utils
    AArch64Info
    OrcJIT
    WindowsDriver
    MCJIT
    JITLink
    Interpreter
    ExecutionEngine
    RuntimeDyld
    OrcTargetProcess
    OrcShared
    DWP
    DebugInfoLogicalView
    DebugInfoGSYM
    Option
    ObjectYAML
    ObjCopy
    MCA
    MCDisassembler
    LTO
    Passes
    CFGuard
    Coroutines
    ipo
    Vectorize
    Linker
    Instrumentation
    FrontendOpenMP
    FrontendOpenACC
    FrontendHLSL
    Extensions
    DWARFLinkerParallel
    DWARFLinker
    GlobalISel
    MIRParser
    AsmPrinter
    SelectionDAG
    CodeGen
    Target
    ObjCARCOpts
    CodeGenTypes
    IRPrinter
    InterfaceStub
    FileCheck
    FuzzMutate
    ScalarOpts
    InstCombine
    AggressiveInstCombine
    TransformUtils
    BitWriter
    Analysis
    ProfileData
    Symbolize
    DebugInfoBTF
    DebugInfoPDB
    DebugInfoMSF
    DebugInfoDWARF
    Object
    TextAPI
    MCParser
    IRReader
    AsmParser
    MC
    DebugInfoCodeView
    BitReader
    FuzzerCLI
    Core
    Remarks
    BitstreamReader
    BinaryFormat
    TargetParser
    TableGen
    Support
    Demangle
)

target_link_libraries(dorset ${llvm_libs})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "-DGIT_BRANCH=\"${GIT_BRANCH}\"")

install(TARGETS dorset PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dorset-lang)

