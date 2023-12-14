add_library(dorsetLexicalAnalysis STATIC
    Lexer.cpp
    Token.cpp
)

target_link_libraries(dorsetDriver dorsetUtils)

target_include_directories(dorsetLexicalAnalysis PRIVATE ../../include)

install(TARGETS dorsetLexicalAnalysis DESTINATION lib)