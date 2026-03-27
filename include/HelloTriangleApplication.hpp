#ifndef HELLOTRIANGLEAPPLICATION_HPP
#define HELLOTRIANGLEAPPLICATION_HPP

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

#include <chrono>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <initializer_list>

#include "vertex.hpp"


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

    std::vector<Vertex> vertices;

    static constexpr int MAX_FRAMES_IN_FLIGHT{ 2 };

    bool frameBufferResized{ false };
    

    GLFWwindow* window{ nullptr };
    
    vk::raii::Context context;
    vk::raii::Instance instance{ nullptr };
    vk::raii::DebugUtilsMessengerEXT debugMessenger{ nullptr };

    /*Timing*/
    std::chrono::steady_clock::time_point startTime;

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

    /*Vertex Buffers*/
    vk::raii::Buffer vertexBuffer{ nullptr };
    vk::raii::DeviceMemory vertexBufferMemory{ nullptr };

    void readVertices(std::initializer_list<Vertex> verticeValues);

    void createInstance();
    
    void initVulkan();
    
    void pickPhysicalDevice();
    
    void createSurface();

    void createLogicalDevice();

    void setupDebugMessenger();

    void recreateSwapChain();

    void cleanupSwapChain();

    void initWindow();
    
    void mainLoop();
    
    void cleanup();

    void createSwapChain();

    void createImageView();

    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

    void drawFrame();

    void createGraphicsPipeline();

    void secondPipeline();

    void createSyncObjects();

    /*Command Buffer Stuff*/
    void createCommandPool();
    void createCommandBuffer();
    void recordCommandBuffer(uint32_t imageIdx);
    void transition_image_layout(uint32_t imageIdx, vk::ImageLayout oldLayout, vk::ImageLayout imageLayout, vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask);

    /*BUFFERS*/
    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags props, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory);
    void copyBuffer(vk::raii::Buffer& stagingBuffer, vk::raii::Buffer& vertexBuffer, vk::DeviceSize size);
    void createVertexBuffer();

    vk::raii::ShaderModule createShaderModule(std::vector<char> const& shader) const;
};


#endif