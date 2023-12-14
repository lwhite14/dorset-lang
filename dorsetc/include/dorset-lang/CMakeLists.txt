set(DORSET_PUBLIC_HEADERS
    AST/AST.h
    Builder/ASTBuilder.h
    Builder/ExpressionBuilder.h
    Driver/CLI.h
    LexicalAnalysis/Lexer.h
    LexicalAnalysis/Token.h
    Utils/Error.h
    Utils/OutputUtils.h
    Utils/Version.h
    catch.hpp
    magic_enum.hpp
    semver.hpp
)

install(FILES AST/AST.h                     DESTINATION include/dorsetDriver)
install(FILES Builder/ASTBuilder.h          DESTINATION include/dorsetDriver)
install(FILES Builder/ExpressionBuilder.h   DESTINATION include/dorsetDriver)
install(FILES Driver/CLI.h                  DESTINATION include/dorsetDriver)
install(FILES LexicalAnalysis/Lexer.h       DESTINATION include/dorsetDriver)
install(FILES LexicalAnalysis/Token.h       DESTINATION include/dorsetDriver)
install(FILES Utils/Error.h                 DESTINATION include/dorsetDriver)
install(FILES Utils/OutputUtils.h           DESTINATION include/dorsetDriver)
install(FILES Utils/Version.h               DESTINATION include/dorsetDriver)
install(FILES catch.hpp                     DESTINATION include/dorsetDriver)
install(FILES magic_enum.hpp                DESTINATION include/dorsetDriver)
install(FILES semver.hpp                    DESTINATION include/dorsetDriver)