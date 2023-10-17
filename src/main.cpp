#include "cli.h"

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT double putchard(double X) {
    fputc((char)X, stderr);
    return 0;
}

int main(int argc, char *argv[])
{
    CompilerOptions options = CompilerOptions(argc, argv);
    Compiler compiler = Compiler(options);
    return compiler.compile();
}
