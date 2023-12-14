add_executable(benchmarkTest BenchmarkTest.cpp)

target_include_directories(benchmarkTest PRIVATE ../../include)

target_link_libraries(benchmarkTest dorsetDriver)




add_executable(compileTest CompileTest.cpp)

target_include_directories(compileTest PRIVATE ../../include)

target_link_libraries(compileTest dorsetDriver)
