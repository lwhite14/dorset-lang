if (WIN32)
    add_executable(${PROJECT_NAME} 
        AppEntry.cpp
        resources/icon.o
    )
else()
    add_executable(${PROJECT_NAME} 
        AppEntry.cpp
    )
endif()

target_include_directories(${PROJECT_NAME}  PRIVATE ../include)

target_link_libraries(${PROJECT_NAME} dorsetDriver)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)