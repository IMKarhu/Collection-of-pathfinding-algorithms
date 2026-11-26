add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
    main.cpp
    raygui_impl.cpp
    grid.h
    grid.cpp
    searchrunner.h
    searchrunner.cpp
    algorithms.h
    algorithms.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE ext)
