find_package(Vulkan REQUIRED)

set(SOURCES
    imgui.h
    imgui.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    backends/imgui_impl_vulkan.cpp
    backends/imgui_impl_sdl2.cpp
    imgui_tables.cpp
)

add_library(imgui STATIC ${SOURCES})

target_include_directories(imgui PRIVATE Vulkan::Vulkan SDL2::SDL2)
target_link_libraries(imgui PRIVATE Vulkan::Vulkan SDL2::SDL2)