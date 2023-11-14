find_package(Vulkan REQUIRED)
option(SDL_TEST OFF)

add_subdirectory(glm)
add_subdirectory(SDL2)
add_subdirectory(enet)
add_subdirectory(entt)
add_subdirectory(stb_image)
add_subdirectory(imgui)
add_subdirectory(assimp)
#ADD_SUBDIRECTORY(RmlUi/freetype)
add_subdirectory(RmlUi)

add_library(vendor INTERFACE)

target_include_directories(vendor INTERFACE SDL2::SDL2 entt RmlCore stb_image glm::glm Vulkan::Vulkan enet/include imgui assimp)
target_link_libraries(vendor INTERFACE SDL2::SDL2 entt RmlCore stb_image glm::glm Vulkan::Vulkan enet imgui assimp)