add_executable(${PROJECT_NAME})

set(SOURCES
        main.cpp
        Scenes/MainScene.h Scenes/MainScene.cpp
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE engine)