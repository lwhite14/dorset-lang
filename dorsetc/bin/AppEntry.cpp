#include <dorset-lang/Driver/CLI.h>

using namespace Dorset;

int main(int argc, char *argv[])
{
    CompilerOptions options = CompilerOptions(argc, argv);
    Compiler compiler = Compiler(options);
    return compiler.compile();
}
