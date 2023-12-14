add_executable(benchmarkTest BenchmarkTest.cpp)

target_include_directories(benchmarkTest PRIVATE ../../../include)
target_include_directories(benchmarkTest PRIVATE ../../../deps)

target_link_libraries(benchmarkTest dorsetDriver)