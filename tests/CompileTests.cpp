#define CATCH_CONFIG_MAIN
#include "../deps/catch.hpp"

#include "../src/CLI/CLI.h"

void resetGlobals()
{
	ErrorHandler::HadError = false;
}


TEST_CASE("Basic Hello World", "[Compile]") // compileTest_1.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_1.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Externs", "[Compile]") // compileTest_2.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_2.ds", "-l"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Returns", "[Compile]") // compileTest_3.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_3.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Variable and If Statement", "[Compile]") // compileTest_4.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_4.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Return from Inside an If Block", "[Compile]") // compileTest_5.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_5.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Inline Block", "[Compile]") // compileTest_6.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_6.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("For Loop", "[Compile]") // compileTest_7.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_7.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Empty Function Body", "[Compile]") // compileTest_8.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_8.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}