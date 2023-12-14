add_executable(compileTest CompileTest.cpp)

target_include_directories(compileTest PRIVATE ../../../include)
target_include_directories(compileTest PRIVATE ../../../deps)

target_link_libraries(compileTest dorsetDriver)
