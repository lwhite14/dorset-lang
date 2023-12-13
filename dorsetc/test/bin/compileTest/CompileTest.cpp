#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <dorsetDriver/CLI/CLI.h>

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

	CompilerOptions options = CompilerOptions({"src/compileTest_2.ds"});

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

TEST_CASE("Returning from For Loop", "[Compile]") // compileTest_9.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_9.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Optional Else in If Statement", "[Compile]") // compileTest_10.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_10.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Basic Arrays", "[Compile]") // compileTest_11.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_11.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("In-Built 'newLine' and 'print' Functions", "[Compile]") // compileTest_12.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_12.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}
