set(DORSET_PUBLIC_HEADERS
    AST/AST.h
    Builder/ASTBuilder.h
    Builder/ExpressionBuilder.h
    CLI/CLI.h
    LexicalAnalysis/Lexer.h
    LexicalAnalysis/Token.h
    Utils/Error.h
    Utils/OutputUtils.h
    Utils/Version.h
)

install(FILES AST/AST.h                     DESTINATION include/dorsetDriver)
install(FILES Builder/ASTBuilder.h          DESTINATION include/dorsetDriver)
install(FILES Builder/ExpressionBuilder.h   DESTINATION include/dorsetDriver)
install(FILES CLI/CLI.h                     DESTINATION include/dorsetDriver)
install(FILES LexicalAnalysis/Lexer.h       DESTINATION include/dorsetDriver)
install(FILES LexicalAnalysis/Token.h       DESTINATION include/dorsetDriver)
install(FILES Utils/Error.h                 DESTINATION include/dorsetDriver)
install(FILES Utils/OutputUtils.h           DESTINATION include/dorsetDriver)
install(FILES Utils/Version.h               DESTINATION include/dorsetDriver)