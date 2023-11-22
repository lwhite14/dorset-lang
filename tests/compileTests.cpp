#define CATCH_CONFIG_MAIN
#include "../deps/catch.hpp"

#include "../src/cli.h"

void resetGlobals()
{
	ErrorHandler::HadError = false;
}


TEST_CASE("Basic Hello World", "[Compile]") 
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_1.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Externs", "[Compile]") 
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_2.ds", "-l"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Returns", "[Compile]") 
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_3.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Variable and If Statement", "[Compile]") 
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_4.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Return from Inside an If Block", "[Compile]") 
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_5.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Inline Block", "[Compile]") 
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_6.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}