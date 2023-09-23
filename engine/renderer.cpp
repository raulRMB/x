//
// Created by Raul Romero on 2023-09-22.
//

#include "renderer.h"


s32 renderer::Init(GLFWwindow *window)
{
    Window = window;

    try
    {
        CreateInstance();
    } catch (const std::runtime_error& e)
    {
        printf("Error: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

renderer::renderer() : Window(nullptr), Instance(VkInstance()) {}

renderer::~renderer()
{
    Window = nullptr;
}

void renderer::CreateInstance()
{
    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = "X";
    AppInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    AppInfo.pEngineName = "XEngine";
    AppInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    AppInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    CreateInfo.pApplicationInfo = &AppInfo;

    std::vector<const char*> InstanceExtensions = std::vector<const char*>();
    u32 GLFWExtensionCount = 0;
    const char** GLFWExtensions = glfwGetRequiredInstanceExtensions(&GLFWExtensionCount);

    for (u32 i = 0; i < GLFWExtensionCount; i++)
    {
        InstanceExtensions.push_back(GLFWExtensions[i]);
    }

    CreateInfo.enabledExtensionCount = static_cast<u32>(InstanceExtensions.size());
    CreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();

    // TODO: Add validation layers
    CreateInfo.enabledLayerCount = 0;
    CreateInfo.ppEnabledLayerNames = nullptr;

    if (vkCreateInstance(&CreateInfo, nullptr, &Instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create Vulkan instance!");
    }
}
