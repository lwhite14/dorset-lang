#define CATCH_CONFIG_MAIN
#include <dorset-lang/catch.hpp>

#include <dorset-lang/Driver/CLI.h>

using namespace Dorset;

void resetGlobals()
{
	ErrorHandler::HadError = false;
}


TEST_CASE("Basic Hello World [1]", "[Compile]") // compileTest_1.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_1.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Externs [2]", "[Compile]") // compileTest_2.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_2.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Returns [3]", "[Compile]") // compileTest_3.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_3.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Variable and If Statement [4]", "[Compile]") // compileTest_4.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_4.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Return from Inside an If Block [5]", "[Compile]") // compileTest_5.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_5.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Inline Block [6]", "[Compile]") // compileTest_6.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_6.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("For Loop [7]", "[Compile]") // compileTest_7.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_7.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Empty Function Body [8]", "[Compile]") // compileTest_8.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_8.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Returning from For Loop [9]", "[Compile]") // compileTest_9.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_9.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Optional Else in If Statement [10]", "[Compile]") // compileTest_10.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_10.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Basic Arrays [11]", "[Compile]") // compileTest_11.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_11.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("In-Built 'newLine' and 'print' Functions [12]", "[Compile]") // compileTest_12.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_12.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("Expression in Array Element Index [13]", "[Compile]") // compileTest_13.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_13.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}

TEST_CASE("'==' Operator [14]", "[Compile]") // compileTest_14.ds
{
	// Pre Work
	resetGlobals();

	CompilerOptions options = CompilerOptions({"src/compileTest_14.ds"});

	REQUIRE(options.getHadError() == false);

	Compiler compiler = Compiler(options);
	int i = compiler.compile();

	REQUIRE(i == 0);
}