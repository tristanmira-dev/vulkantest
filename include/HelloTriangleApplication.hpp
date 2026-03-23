#ifndef HELLOTRIANGLEAPPLICATION_HPP
#define HELLOTRIANGLEAPPLICATION_HPP

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    glm::mat4x4 proj;

    static constexpr int MAX_FRAMES_IN_FLIGHT{ 2 };
    

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
    uint32_t queueIdx = ~0;

    
    /*Swap chain*/
    vk::raii::SwapchainKHR swapChain{ nullptr };
    std::vector<vk::Image> swapChainImages;
    vk::SurfaceFormatKHR swapChainSurfaceFormat;
    vk::Extent2D swapChainExtent;
    
    /*Image Views*/
    std::vector<vk::raii::ImageView> swapChainImageViews;

    /*Pipeline*/
    vk::raii::PipelineLayout pipelineLayout{ nullptr };
    vk::raii::Pipeline pipeline{ nullptr };
    /*Second Pipeline*/
    vk::raii::PipelineLayout pipelineLayout2{ nullptr };
    vk::raii::Pipeline pipeline2{ nullptr };

    /*Command buffer object*/
    vk::raii::CommandPool commandPool{ nullptr };
    std::vector<vk::raii::CommandBuffer> commandBuffers;

    /*Frames in flight*/
    uint32_t frameIdx{};

    /*Synchronization objects*/
    /*Semaphores for now are mainly for swapchain operations, and Fences are mainly for drawing frames (do not want to override the command buffers before the previous drawFrame calls while the gpu is still processing it)*/
    std::vector<vk::raii::Semaphore> presentCompleteSemaphore;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphore;
    std::vector<vk::raii::Fence> inFlightFences;

    std::vector<vk::raii::Semaphore> secondPipelineSemaphore2;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphore2;


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

    void drawFrame();

    void createGraphicsPipeline();

    void secondPipeline();

    void createSyncObjects();

    /*Command Buffer Stuff*/
    void createCommandPool();
    void createCommandBuffer();
    void createCommandPool2();
    void createCommandBuffer2();
    void recordCommandBuffer(uint32_t imageIdx);
    void recordCommandBuffer2(uint32_t imageIdx);
    void transition_image_layout(uint32_t imageIdx, vk::ImageLayout oldLayout, vk::ImageLayout imageLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask, int offset = 0);

    /**/

    vk::raii::ShaderModule createShaderModule(std::vector<char> const& shader) const;
};


#endif