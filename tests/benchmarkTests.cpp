#include <chrono>

#include "../src/cli.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;
using std::chrono::_V2::system_clock;

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
    system("src/benchmarkTest_1.out");
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
    system("src/benchmarkTest_2.out");
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
    system("src/benchmarkTest_3.out");
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
    system("src/benchmarkTest_4.out");
}

void timeBenchmark1()
{
    std::cout << "Source File 1" << std::endl;

    system_clock::time_point t1;
    system_clock::time_point t2;

    t1 = high_resolution_clock::now();
    compileTest1();
    t2 = high_resolution_clock::now();
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

    system_clock::time_point t1;
    system_clock::time_point t2;

    t1 = high_resolution_clock::now();
    compileTest2();
    t2 = high_resolution_clock::now();
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

    system_clock::time_point t1;
    system_clock::time_point t2;

    t1 = high_resolution_clock::now();
    compileTest3();
    t2 = high_resolution_clock::now();
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

    system_clock::time_point t1;
    system_clock::time_point t2;

    t1 = high_resolution_clock::now();
    compileTest4();
    t2 = high_resolution_clock::now();
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