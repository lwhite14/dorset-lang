#include "cli.h"

int main(int argc, char *argv[])
{
    CompilerOptions options = CompilerOptions(argc, argv);
    Compiler compiler = Compiler(options);
    return compiler.compile();
}
