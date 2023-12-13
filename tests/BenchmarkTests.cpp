#include <chrono>

#include "../src/CLI/CLI.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void resetGlobals()
{
	ErrorHandler::HadError = false;
}

void compileTest1()
{
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/benchmarkTest_1.ds"});

	Compiler compiler = Compiler(options);
	int i = compiler.compile();
}

void runTest1()
{
#if defined(_WIN64) || defined(_WIN32)
    system("\"benchmarkTest_1.out\"");
#else
    system("benchmarkTest_1.out");
#endif
}

void compileTest2()
{
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/benchmarkTest_2.ds"});

	Compiler compiler = Compiler(options);
	int i = compiler.compile();
}

void runTest2()
{
#if defined(_WIN64) || defined(_WIN32)
    system("\"benchmarkTest_2.out\"");
#else
    system("benchmarkTest_2.out");
#endif
}

void compileTest3()
{
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/benchmarkTest_3.ds"});

	Compiler compiler = Compiler(options);
	int i = compiler.compile();
}

void runTest3()
{
#if defined(_WIN64) || defined(_WIN32)
    system("\"benchmarkTest_3.out\"");
#else
    system("benchmarkTest_3.out");
#endif
}

void compileTest4()
{
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/benchmarkTest_4.ds"});

	Compiler compiler = Compiler(options);
	int i = compiler.compile();
}

void runTest4()
{
#if defined(_WIN64) || defined(_WIN32)
    system("\"benchmarkTest_4.out\"");
#else
    system("benchmarkTest_4.out");
#endif
}

void timeBenchmark1()
{
    std::cout << "Source File 1" << std::endl;

    auto t1 = high_resolution_clock::now();
    compileTest1();
    auto t2 = high_resolution_clock::now();
    auto durationCompile = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Compilation Time: " << durationCompile.count() << "ms" << std::endl;

    t1 = high_resolution_clock::now();
    runTest1();
    t2 = high_resolution_clock::now();
    auto durationExecute = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Execution Time: "<< durationExecute.count() << "ms" << std::endl;
    std::cout << std::endl;
}

void timeBenchmark2()
{
    std::cout << "Source File 2" << std::endl;

    auto t1 = high_resolution_clock::now();
    compileTest2();
    auto t2 = high_resolution_clock::now();
    auto durationCompile = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Compilation Time: " << durationCompile.count() << "ms" << std::endl;

    t1 = high_resolution_clock::now();
    runTest2();
    t2 = high_resolution_clock::now();
    auto durationExecute = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Execution Time: "<< durationExecute.count() << "ms" << std::endl;
    std::cout << std::endl;
}

void timeBenchmark3()
{
    std::cout << "Source File 3" << std::endl;

    auto t1 = high_resolution_clock::now();
    compileTest3();
    auto t2 = high_resolution_clock::now();
    auto durationCompile = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Compilation Time: " << durationCompile.count() << "ms" << std::endl;

    t1 = high_resolution_clock::now();
    runTest3();
    t2 = high_resolution_clock::now();
    auto durationExecute = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Execution Time: "<< durationExecute.count() << "ms" << std::endl;
    std::cout << std::endl;
}

void timeBenchmark4()
{
    std::cout << "Source File 4" << std::endl; 

    auto t1 = high_resolution_clock::now();
    compileTest4();
    auto t2 = high_resolution_clock::now();
    auto durationCompile = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Compilation Time: " << durationCompile.count() << "ms" << std::endl;

    t1 = high_resolution_clock::now();
    runTest4();
    t2 = high_resolution_clock::now();
    auto durationExecute = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Execution Time: "<< durationExecute.count() << "ms" << std::endl;
    std::cout << std::endl;
}

int main()
{
    timeBenchmark1();
    timeBenchmark2();
    timeBenchmark3();
    timeBenchmark4();

    return 0;
}