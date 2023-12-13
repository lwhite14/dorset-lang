# Changelog

All notable changes to this project will be documented in this file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

Unreleased is the develop branch which includes every unmerged change from the last release version. These changes will be untested and may be potentially unstable, build this version at your own risk.

## [0.2.0-alpha] - 2023-12-13

### Added
 - Install docs pages.
 - Optional else block for if statements.
 - Users can choose which binary to compile the final object file.
 - Very simple arrays.
 - In-built newLine function.
 - In-built print function.

### Fixed
 - Final outputs binaries are outputted in the same directory that dorsetc is called in.

### Changed 
 - Renamed compiler to 'dorsetc'
 - Compile to object file directly without system calls.
 - Module ID for LLVM IR file is now just the file name.
 - dorsetc outputs .exe and .obj files for Windows.


## [0.1.5-alpha] - 2023-11-25

### Added
 - Can add raw code directly into the CLI.
 - Some basic syntax tests.
 - Some basic benchmark tests.
 - Returning from inside an if block.
 - Multi-line function blocks.

### Changed
 - Seperated compilation driver from final compiler binary.
 - Block parsing. Will allow for better if and for token parsing.
 - New source directory layout.
 - More robust keyword and type lexer parsing.


## [0.1.4-alpha] - 2023-11-07

### Added
 - This here changelog!
 - Development build warning.
 - Error messaging colours.
 - Error message if code generation fails.
 - Multi line functions.
 - Basic return types (void and double).
 - Documentation page.
 - Executable icon for windows binary.

### Fixed
 - Unescapable loop when code gets stuck returning the 'end of expression' token.

### Changed
 - Improved error messaging formatting.
 - Improved token output formatting.
 - Made the variable declaration a lot simpler: 'var i = 3;'
 - If statements now use curly braces.
 - For loops now use curly braces.

## [0.1.3-alpha] - 2023-10-28

### Added
 - Can explicitely output new lines by using printf("\n")
 - Mutable variables.
 - Added extern functions.
 - Comma sperated function prototypes. 
 - [Icons](branding/icon.png).

### Fixed
 - The long list of errors users get when only one error is really caught.
 - The seg fault when no output given when using -o option.

### Removed
 - no-pie option for Windows machines, as it wasn't needed.

## [0.1.2-alpha] - 2023-10-22

### Added
 - Operator overloading
    - Users can now create unary and binary operators.
 - no-pie flag for Linux compilation.
 - If statiements.
 - For loops.
 - printf external functions for doubles.
 - Install location for cmake.
 - MasterAST class to hold important LLVM objects needed across classes.

### Fixed
 - The ASTBuilder class previously incorrectly found lexeme characters, this was brought to light when trying to find the operator lexeme, this has been fixed. 

### Changed
 - The way functions are found, the Compiler will now search for function prototypes is the function is not found. 
 - Moved initialiseModule, outputBinaries, and removeBinaries to the Compiler class. This removed the need for static binary name variables.
 - Use std::move, so that deleted pointers won't cause a cascade of issues.
 - Replaced gcc with clang for final binary compilation.
 - Use of fileExists function for removing output binaries.

## [0.1.1-alpha] - 2023-10-14

### Added
 - Full names of compiler flags (e.g. -t == --tokens).
 - More compiler flags:
    - -o: change the output binaries name.
    - -l: option to compile to library file.
    - -r: keep the LLVM IR generated code.
 - Compiler class to handle top level compiler related functionality.
 - Output names to Compiler options, output binaries names can be changed based on these.
 - A FunctionPassManager which optimises LLVM IR function code.

### Changed
 - Moved the application entry into the Compiler class, this class now handles most of the top level functionality.

### Removed
 - Most of the top level app entry related code from main.cpp.
 - Removed need to pass in fileName and filePath into initializeModule function.


## [0.1.0-alpha] - 2023-10-10

### Added
 - Lexical analysis of source file.
 - Abstract syntax tree building.
    - AST includes:
        - Base Expresions.
        - Numbers (doubles by default).
        - Strings.
        - Variables (only parameters).
        - Binary expressions.
        - Calls.
        - Prototypes.
        - Functions.
 - Output into LLVM IR and object files.
 - Error logging system.
 - printf external function.
 - CompilerOptions class for determining compiler flags.
    - Flags include:
        - -t: for printing the tokens of a source file.
        - -h: printing the usage.
        - -v: printing the version.


[unreleased]: https://github.com/lwhite14/dorset-lang/compare/0.2.0-alpha...develop
[0.2.0-alpha]: https://github.com/lwhite14/dorset-lang/releases/tag/0.2.0-alpha
[0.1.5-alpha]: https://github.com/lwhite14/dorset-lang/releases/tag/0.1.5-alpha
[0.1.4-alpha]: https://github.com/lwhite14/dorset-lang/releases/tag/0.1.4-alpha
[0.1.3-alpha]: https://github.com/lwhite14/dorset-lang/releases/tag/0.1.3-alpha
[0.1.2-alpha]: https://github.com/lwhite14/dorset-lang/releases/tag/0.1.2-alpha
[0.1.1-alpha]: https://github.com/lwhite14/dorset-lang/releases/tag/0.1.1-alpha
[0.1.0-alpha]: https://github.com/lwhite14/dorset-lang/releases/tag/0.1.0-alpha