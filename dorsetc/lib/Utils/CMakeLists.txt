add_library(dorsetUtils STATIC
    Error.cpp
    OutputUtils.cpp
    Version.cpp
)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

target_compile_definitions(dorsetUtils PRIVATE "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")
target_compile_definitions(dorsetUtils PRIVATE "-DGIT_BRANCH=\"${GIT_BRANCH}\"")

target_link_libraries(dorsetDriver dorsetLexicalAnalysis)

target_include_directories(dorsetUtils PRIVATE ../../include)

install(TARGETS dorsetUtils DESTINATION lib)