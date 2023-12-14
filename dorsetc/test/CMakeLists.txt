add_subdirectory(bin)

enable_testing()
add_test(NAME compileTest COMMAND $<TARGET_FILE:compileTest>)
add_test(NAME benchmarkTest COMMAND $<TARGET_FILE:benchmarkTest>)
file(COPY src DESTINATION ${CMAKE_CURRENT_BINARY_DIR})