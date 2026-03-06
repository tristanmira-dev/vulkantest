#ifndef HELLOTRIANGLEAPPLICATION_HPP
#define HELLOTRIANGLEAPPLICATION_HPP

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

class HelloTriangleApplication {
public:
    void run();

private:

    const std::vector<char const*> validationLayers{
        "VK_LAYER_KHRONOS_validation"
    };

    std::vector<const char*> deviceExtensions = {
        vk::KHRSwapchainExtensionName /*extension of the gpu for presenting to the monitor*/
    };

    

    GLFWwindow* window{ nullptr };
    
    vk::raii::Context context;
    vk::raii::Instance instance{ nullptr };
    vk::raii::DebugUtilsMessengerEXT debugMessenger{ nullptr };

    /*Graphics card*/
    vk::raii::Device device{ nullptr }; /*Logical Device*/
    vk::raii::Queue graphicsQueue{ nullptr };
    vk::raii::Queue presentQueue{ nullptr };
    vk::raii::PhysicalDevice physicalDevice{ nullptr };
    vk::raii::SurfaceKHR surface{ nullptr };
    
    /*Swap chain*/
    vk::raii::SwapchainKHR swapChain{ nullptr };
    std::vector<vk::Image> swapChainImages;
    vk::Format swpChainImageFormat{vk::Format::eUndefined};
    vk::SurfaceFormatKHR swapChainSurfaceFormat;
    vk::Extent2D swapChainExtent;
    
    /*Image Views*/
    std::vector<vk::raii::ImageView> swapChainImageViews;

    void createInstance();
    
    void initVulkan();
    
    void pickPhysicalDevice();
    
    void createSurface();

    void createLogicalDevice();

    void setupDebugMessenger();

    void initWindow();
    
    void mainLoop();
    
    void cleanup();

    void createSwapChain();


    void createImageView();
};


#endif