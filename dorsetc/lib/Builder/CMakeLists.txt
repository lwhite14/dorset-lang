add_library(dorsetBuilder STATIC
    ASTBuilder.cpp
    ExpressionBuilder.cpp
)

target_link_libraries(dorsetBuilder dorsetLexicalAnalysis)
target_link_libraries(dorsetBuilder dorsetAST)
target_link_libraries(dorsetBuilder dorsetUtils)

target_include_directories(dorsetBuilder PRIVATE ../../include)

install(TARGETS dorsetBuilder DESTINATION lib)