set(DORSET_EXTERNAL_HEADERS
    catch.hpp
    magic_enum.hpp
    semver.hpp
)

install(FILES catch.hpp                     DESTINATION include)
install(FILES magic_enum.hpp                DESTINATION include)
install(FILES semver.hpp                    DESTINATION include)