add_executable(${PROJECT_NAME})

set(SOURCES
    main.cpp
    Scenes/MainScene.h Scenes/MainScene.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} engine)
target_link_directories(${PROJECT_NAME} PRIVATE engine)
target_link_libraries(${PROJECT_NAME} PRIVATE engine)